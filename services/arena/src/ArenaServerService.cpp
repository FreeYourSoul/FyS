// MIT License
//
// Copyright (c) 2019 Quentin Balland
// Repository : https://github.com/FreeYourSoul/FyS
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
//         of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
//         to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//         copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
//         copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//         AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <spdlog/spdlog.h>
#include <functional>
#include <zmq_addon.hpp>
#include <flatbuffers/flatbuffers.h>
#include <FightingPitEncounter_generated.h>
#include <ArenaServerAuth_generated.h>
#include <ArenaServerValidateAuth_generated.h>
#include <ArenaServerContext.hh>
#include "ArenaServerService.hh"

// anonymous namespace used for utility function to extract data from flatbuffer
namespace {

    fys::arena::FightingPit::Level translateLevelFromFlatbuffer(const fys::fb::Level &level) {
        switch (level) {
            case fys::fb::Level_EASY:
                return fys::arena::FightingPit::EASY;
            case fys::fb::Level_MEDIUM:
                return fys::arena::FightingPit::MEDIUM;
            case fys::fb::Level_HARD:
                return fys::arena::FightingPit::HARD;
            default:
                return fys::arena::FightingPit::EASY;
        }
    }

    fys::arena::AwaitingPlayerArena createAwaitingPlayer(const fys::fb::FightingPitEncounter *binary) {
        std::optional<fys::arena::AwaitingArena> awaitingArena = std::nullopt;

        // if fighting_pit_id is 0, a new arena has to be generated furthermore data are extracted
        if (!binary->fighting_pit_id()) {
            awaitingArena = fys::arena::AwaitingArena {
                binary->world_server_id()->str(),
                binary->is_ambush(),
                binary->id_encounter(),
                translateLevelFromFlatbuffer(binary->level_encounter())
            };
        }
        return fys::arena::AwaitingPlayerArena {
                binary->user_name()->str(),
                binary->token_auth()->str(),
                binary->fighting_pit_id(),
                std::move(awaitingArena)
        };
    }

}

namespace fys::arena {

    ArenaServerService::ArenaServerService(const ArenaServerContext &ctx) :
      _ctx(ctx), _cache(ctx.getPathLocalStorageCache(), ctx.getPathSourceCache()) {
        _connectionHandler.setupConnectionManager(ctx);
    }

    void ArenaServerService::runServerLoop() noexcept {
        SPDLOG_INFO("ArenaServer loop started");

        while (true) {
            _connectionHandler.pollAndProcessMessageFromDispatcher(
                    [this](zmq::message_t && identityWs, zmq::message_t && worldServerMessage) {
                        const auto *binary = fys::fb::GetFightingPitEncounter(worldServerMessage.data());

                        // register player incoming into arena instance with token given by worldserver used as key
                        const auto &[elem, playerHasBeenCorrectlyRegistered] = _awaitingArena.insert(
                                { binary->token_auth()->str(), createAwaitingPlayer(binary) }
                            );
                        if (playerHasBeenCorrectlyRegistered)
                            forwardReplyToDispatcherClient(std::move(identityWs), elem->second);
                    }
            );

            _workerService.pollAndProcessMessageFromPlayer(
                    // Authentication handler
                    [this](zmq::message_t && identityPlayer, zmq::message_t && authMessage) {
                        const auto *binary = fys::fb::GetArenaServerValidateAuth(authMessage.data());
                        std::string userName = binary->user_name()->str();
                        const std::string tokenAuth = binary->token_auth()->str();
                        const auto &[isAwaited, playerAwaitedIt] = isPlayerAwaited(userName, tokenAuth, binary->fighting_pit_id());
                        unsigned fightingPitId = playerAwaitedIt->second.fightingPitId;
                        zmq::multipart_t response;

                        response.add(std::move(identityPlayer));
                        if (!isAwaited) {
                            spdlog::warn("Player {} tried to authenticate on Arena server {} without being awaited.",
                                    userName, _ctx.get().getServerCode());
                            return;
                        }
                        if (playerAwaitedIt->second.hasToBeGenerated())
                            fightingPitId = createNewFightingPit(playerAwaitedIt);
                        else
                            _workerService.playerJoinFightingPit(std::move(userName), fightingPitId);
                    },

                    // InGame handler
                    [this](zmq::message_t && identityPlayer, zmq::message_t && playerMessage) {
                        // todo
                        // deserialize playerMessage
                        // Check if the player is authenticated on the fightingpit thanks to the token
                        // forward the message
                        zmq::multipart_t response;
                        response.add(std::move(identityPlayer));
                    }
            );
        }
    }

    std::pair<bool, const std::unordered_map<std::string, AwaitingPlayerArena>::const_iterator>
    ArenaServerService::isPlayerAwaited(const std::string &name, const std::string &token, unsigned idFightingPit) const {
        if (const auto it = _awaitingArena.find(token);
        it != _awaitingArena.end() && it->second.namePlayer == name && it->second.fightingPitId == idFightingPit)
            return { true, it };
        return {false, _awaitingArena.cend() };
    }

    void ArenaServerService::forwardReplyToDispatcherClient(zmq::message_t && identityWs, const fys::arena::AwaitingPlayerArena & awaitingArena) {
        flatbuffers::FlatBufferBuilder fbb;
        auto asaFb = fys::fb::CreateArenaServerAuth(
                fbb,
                fbb.CreateString(awaitingArena.namePlayer),
                fbb.CreateString(awaitingArena.token),
                fbb.CreateString(_ctx.get().getHostname()),
                fbb.CreateString(_ctx.get().getConnectionString()),
                _ctx.get().getPort(),
                awaitingArena.gen ? fbb.CreateString(awaitingArena.gen->serverCode) : fbb.CreateString(""));
        fys::fb::FinishArenaServerAuthBuffer(fbb, asaFb);
        zmq::multipart_t msg;
        msg.add(std::move(identityWs));
        msg.addmem(fbb.GetBufferPointer(), fbb.GetSize());
        _connectionHandler.sendMessageToDispatcher(std::move(msg));
    }

    unsigned ArenaServerService::createNewFightingPit(AwaitingPlayerArenaIt it) {
        FightingPitAnnouncer fpa(_cache);

        fpa.setDifficulty(it->second.gen->levelFightingPit);
        fpa.setEncounterId(it->second.gen->encounterId);
        fpa.enforceAmbush(it->second.gen->isAmbush);
        fpa.generateAllyPartyTeam(it->second.namePlayer, it->second.token);
        unsigned id = _workerService.addFightingPit(
                fpa.buildFightingPit(_ctx.get().getEncounterContext(), _connectionHandler, it->second.gen->serverCode));
        // remove player from awaited player
        _awaitingArena.erase(it);
        return id;
    }


}
