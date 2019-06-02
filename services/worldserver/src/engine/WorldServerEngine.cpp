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
#include <WorldServerContext.hh>
#include <WSAction_generated.h>
#include <Notifications_generated.h>
#include <ConnectionHandler.hh>
#include "engine/WorldServerEngine.hh"

namespace fys::ws {

    WorldServerEngine::WorldServerEngine(const fys::ws::WorldServerContext &ctx) : _map(ctx) {
    }

    void WorldServerEngine::processPlayerInputMessage(std::string &&idt, std::string &&token,
            const fb::WSAction *actionMsg, ConnectionHandler &handler)
    {
        if (const uint index = _data.getIndexAndUpdatePlayerConnection(token, idt); index < std::numeric_limits<uint>::max()) {
            if (actionMsg->action_type() == fb::Action::Action_Move) {
                auto clientToNotify = movePlayerAction(std::move(idt), index, actionMsg->action_as_Move());
                if (!clientToNotify.empty())
                    notifyClientOfMove(clientToNotify, handler);
            }
            else if (actionMsg->action_type() == fb::Action::Action_PnjInteract)
                forwardMessageToOtherServer(std::move(idt), std::move(token), actionMsg->action_as_PnjInteract(), handler);
        } else {
            SPDLOG_ERROR("Player of token {} has not been registered before sending messages", token);
        }
    }

    std::vector<std::string_view> WorldServerEngine::movePlayerAction(std::string &&idt, uint indexPlayer, const fb::Move *action)
    {
        Coordinate &currentPos = _data.getPlayerPosition(indexPlayer);
        double angle = action->direction();
        double velocity = 1;
        Coordinate futurePos = {
                currentPos.x * (velocity * std::cos(angle)),
                currentPos.y * (velocity * std::sin(angle))
        };

        if (_map.canMoveTo(futurePos.x, futurePos.y, 0)) {
            currentPos.x = futurePos.x;
            currentPos.y = futurePos.y;
            _map.executePotentialTrigger();
            return _data.getPlayerIdtsArroundPos(currentPos);
        }
    }

    void WorldServerEngine::notifyClientOfMove(const std::vector<std::string_view> &ids, ws::ConnectionHandler &conn) const
    {
        for (const auto &id : ids) {
//            fys::fb::WSActionNotification notification;
        }
    }

    void WorldServerEngine::forwardMessageToOtherServer(std::string &&idt, std::string &&token,
            const fys::fb::PnjInteract *action, ws::ConnectionHandler &handler) const
    {
        zmq::multipart_t msgToForward;
//        msgToForward.addstr()
        handler.sendMessageToDispatcher(std::move(msgToForward));
    }
}