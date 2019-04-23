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


#ifndef FYS_CONNECTIONHANDLER_HH
#define FYS_CONNECTIONHANDLER_HH

#include <spdlog/spdlog.h>
#include <zmq_addon.hpp>
#include "WorldServerContext.hh"

namespace fys::ws {

    class ConnectionHandler {

    public:
        explicit ConnectionHandler(int threadNumber = 1) noexcept;

        void setupConnectionManager(const fys::ws::WorldServerContext& ctx) noexcept;
        void sendMessageToDispatcher(zmq::multipart_t &&msg) noexcept;

        template <typename Handler>
        void pollAndProcessSubMessage(Handler handler) noexcept {
            //  Initialize poll set
            zmq::pollitem_t items[] = {
                    { _subSocket, 0, ZMQ_POLLIN, 0 }
            };
            zmq::poll(&items[0], 1, -1);
            if (static_cast<bool>(items[0].revents & ZMQ_POLLIN)) {
                zmq::multipart_t msg;
                if (!msg.recv(_subSocket))
                    spdlog::get("c")->error("Error while reading on the listener socket");
                else
                    handler(std::move(msg));
            }
        }

    private:
        zmq::context_t _zmqContext;
        zmq::socket_t _subSocket;
        zmq::socket_t _dispatcherConnection;

    };

}

#endif //FYS_CONNECTIONHANDLER_HH
