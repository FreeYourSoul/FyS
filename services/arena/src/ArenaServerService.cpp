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
#include <ArenaServerContext.hh>
#include "ArenaServerService.hh"

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
               [this](zmq::multipart_t && worldServerMessage) {

                   // register player incoming into arena instance
                   _awaitingArena["token"] = {
                           "name",
                           "WS_Code",           // World Server Code
                           true,                // isAmbush
                           0,                   // Encounter Code
                           FightingPit::EASY    // Level Difficulty
                   };
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
