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
           _connectionHandler.pollAndProcessSubMessage(
               [this](zmq::multipart_t &&msg) {
                   // if new arena request, create the new arena

                   // get id of the arena from message to forward the message to the correct arena instance

                   // register player incoming into arena instance (done for new arena or if the incoming
                   // message is a joining message)

               }
            );
        }
    }

    void ArenaServerService::processMessage(std::string &&idt, std::string &&token, const zmq::message_t &content) {
        // if (request new fighting) {
            FightingPitAnnouncer fpa(_cache);

            // Retrieving data from newFightRequest
            FightingPitAnnouncer::EncounterType encounterType = FightingPitAnnouncer::EncounterType::RANDOM;
            FightingPit::Level level = FightingPit::EASY;
            const std::string serverCode = "WS00";
            bool ambush = false;

            fpa.setDifficulty(level);
            fpa.setEncounterType(encounterType);
            fpa.enforceAmbush(ambush);
            _workerService.addFightingPit(fpa.buildFightingPit(_ctx.get().getEncounterContext(), _connectionHandler, serverCode));
        // }
        // else {
            unsigned fightingPitId = 1; // todo get the correct if from the content of the zmq message
            //FightingPitMessage fightingPitMessage;
            _workerService.forwardMessageToFightingPit(fightingPitId/*, fightingPitMessage*/);
        // }
    }

}
