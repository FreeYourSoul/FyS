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

#include <ArenaServerContext.hh>
#include "ConnectionHandler.hh"

namespace fys::arena {

    static const fys::arena::ArenaServerContext *ctxw;


    void ConnectionHandler::setupConnectionManager(const fys::arena::ArenaServerContext &ctx) noexcept {
        // todo add a heartbeat to re-connect to dispatcher if no message after some time?
        ctxw = &ctx;
        // connect to dispatcher with the dispatcher
        _dealerConnectionToDispatcher.connect(ctx.getDispatcherConnectionString());
    }

    void ConnectionHandler::sendMessageToDispatcher(zmq::multipart_t && msg) noexcept {
        if (_dealerConnectionToDispatcher.connected()) {
            SPDLOG_INFO("Message send to dispatcher {} \n", msg.str());
            msg.send(_dealerConnectionToDispatcher);
        }
    }

}
