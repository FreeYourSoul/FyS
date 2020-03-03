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
#include <ArenaServerContext.hh>
#include <ArenaServerAuth_generated.h>
#include "ArenaServerService.hh"

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

                        // register player incoming into arena instance
                        const auto &[elem, playerHasBeenCorrectlyRegistered] = _awaitingArena.insert(
                            {
                                // authentication token given by worldserver used as key
                                binary->token_auth()->str(),

                                // Creation of the AwaitingPlayerArena representing an awaited player
                                AwaitingPlayerArena {
                                        binary->user_name()->str(),
                                        binary->token_auth()->str(),
                                        AwaitingPlayerArena::GENERATE_ARENA,
                                        AwaitingArena {
                                                binary->world_server_id()->str(),
                                                binary->is_ambush(),
                                                binary->id_encounter(),
                                                translateLevelFromFlatbuffer(binary->level_encounter())
                                        }
                                }
                            }
                        );
                        if (playerHasBeenCorrectlyRegistered)
                            forwardReplyToDispatcherClient(std::move(identityWs), elem->second);
                    }
            );

            _workerService.pollAndProcessMessageFromPlayer(
                    // Authentication handler
                    [this](zmq::message_t && identityPlayer, zmq::message_t && authMessage) {
                        // todo
                        // deserialize authMessage
                        // Check if the player is awaited thanks to the token
                        // Create FightingPit if the awaited player is a creator
                        // Join the requested FightingPit otherwise
                    },

                    // InGame handler
                    [this](zmq::message_t && identityPlayer, zmq::message_t && playerMessage) {
                        // todo
                        // deserialize playerMessage
                        // Check if the player is authenticated on the fightingpit thanks to the token
                        // forward the message

                    }
            );
        }
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
                fbb.CreateString(awaitingArena.gen->serverCode));
        fys::fb::FinishArenaServerAuthBuffer(fbb, asaFb);
        zmq::multipart_t msg;
        msg.add(std::move(identityWs));
        msg.addmem(fbb.GetBufferPointer(), fbb.GetSize());
        _connectionHandler.sendMessageToDispatcher(std::move(msg));
    }

    void ArenaServerService::createNewFightingPit(AwaitingPlayerArena && awaited) {
        FightingPitAnnouncer fpa(_cache);

        fpa.setDifficulty(awaited.gen->levelFightingPit);
        fpa.setEncounterId(awaited.gen->encounterId);
        fpa.enforceAmbush(awaited.gen->isAmbush);
        fpa.setCreatorUserName(std::move(awaited.namePlayer));
        _workerService.addFightingPit(
                fpa.buildFightingPit(_ctx.get().getEncounterContext(), _connectionHandler, awaited.gen->serverCode));
    }

}
