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
#include <ProtocolHelper.hh>
#include "ArenaServerService.hh"

namespace {
    fys::arena::FightingPit::Level levelDifficultyFromFb(const fys::fb::Level &level) {
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

    void forwardReplyToDispatcherClient(const fys::arena::AwaitingArena & awaitingArena) {
        flatbuffers::FlatBufferBuilder fbb;
        auto asaFb = fys::fb::CreateArenaServerAuth(
                fbb,
                fbb.CreateString(awaitingArena._namePlayer),
                fbb.CreateString(awaitingArena._token),
                fbb.CreateString("localhost"),
                fbb.CreateString("tcp://localhost:4242"),
                42,
                fbb.CreateString(awaitingArena._serverCode));
        fys::fb::FinishArenaServerAuthBuffer(fbb, asaFb);
        const fys::fb::ArenaServerAuth *asa = fys::fb::GetArenaServerAuth(fbb.GetBufferPointer());;
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
            // parse message when coming from world server
           _connectionHandler.pollAndProcessSubMessage(
               [this](std::string && wsDispatcherRouterIdentity, zmq::message_t && worldServerMessage) {
                   const auto *binary = fys::fb::GetFightingPitEncounter(worldServerMessage.data());

                   // register player incoming into arena instance
                   const auto [elem, hasBeenInserted] = _awaitingArena.insert(
                       {
                        binary->token_auth()->str(),
                        AwaitingArena {
                               binary->user_name()->str(),
                               binary->token_auth()->str(),
                               binary->world_server_id()->str(),
                               binary->is_ambush(),
                               binary->id_encounter(),
                               levelDifficultyFromFb(binary->level_encounter())
                           }
                       }
                   );
                   // if a new player has been registered
                   if (hasBeenInserted)
                       forwardReplyToDispatcherClient(elem);
               }
            );

           // parse message when coming from players
           _workerService.pollAndProcessPlayerMessage(
               [this](zmq::multipart_t && playerMessage){

                }
           );
        }
    }

    void ArenaServerService::createNewFightingPit(AwaitingArena && arenaToCreate) {
        FightingPitAnnouncer fpa(_cache);

        fpa.setDifficulty(arenaToCreate._levelFightingPit);
        fpa.setEncounterId(arenaToCreate._encounterId);
        fpa.enforceAmbush(arenaToCreate._isAmbush);
        fpa.setCreatorUserName(std::move(arenaToCreate._namePlayer));
        _workerService.addFightingPit(
                fpa.buildFightingPit(
                        _ctx.get().getEncounterContext(),
                        _connectionHandler,
                        arenaToCreate._serverCode));
    }

    void ArenaServerService::processMessage(std::string && idt, std::string && token, const zmq::message_t & content) {
        unsigned fightingPitId = 1; // todo get the correct if from the content of the zmq message
        //FightingPitMessage fightingPitMessage;
        _workerService.forwardMessageToFightingPit(fightingPitId/*, fightingPitMessage*/);
    }

}
