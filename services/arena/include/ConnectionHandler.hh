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


#ifndef FYS_ARENA_CONNECTIONHANDLER_HH
#define FYS_ARENA_CONNECTIONHANDLER_HH

#include <spdlog/spdlog.h>
#include <zmq_addon.hpp>

namespace fys::arena {

    class ArenaServerContext;

    class ConnectionHandler {

    public:
        explicit ConnectionHandler(int threadNumber = 1) noexcept :
                _zmqContext(threadNumber),
                _subConnectionToDispatcher(_zmqContext, zmq::socket_type::sub),
                _dealerConnectionToDispatcher(_zmqContext, zmq::socket_type::dealer)
        { }

        /**
         * @brief Connect to the Arena dispatcher in order to receive request from the WS to create new fightingPit
         */
        void setupConnectionManager(const fys::arena::ArenaServerContext &ctx) noexcept;
        void sendMessageToDispatcher(zmq::multipart_t && msg) noexcept;

        /**
         * Read on the subscriber socket (connection with the dispatcher)
         * and reply to dispatcher with the dealer socket connection (which will forward the message the the proper WorldServer)
         * @tparam Lambda type following the signature => void (string, zmq::message_t)
         * @param handler Handler handler to call when receiving a message
         */
        template <typename Handler>
        void pollAndProcessSubMessage(Handler && handler) noexcept {
            //  Initialize poll set
            zmq::pollitem_t items[] = {
                    { _dealerConnectionToDispatcher, 0, ZMQ_POLLIN, 0 }
            };
            zmq::poll(&items[0], 1);
            if (static_cast<bool>(items[0].revents & ZMQ_POLLIN)) {
                zmq::multipart_t msg;
                if (!msg.recv(_dealerConnectionToDispatcher)) {
                    SPDLOG_ERROR("Error while reading on the listener socket");
                }
                else {
                    // first element is the identity frame for the router of the dispatcher (connected to WS)
                    // second element is the message frame
                    std::forward<Handler>(handler)(msg.popstr(), msg.pop());
                }
            }
        }

    private:
        zmq::context_t _zmqContext;

        // read from (connection to get data from the dispatcher)
        zmq::socket_t _subConnectionToDispatcher;
        // write to reply (forwarded to the world server)
        zmq::socket_t _dealerConnectionToDispatcher;

    };

}


#endif //FYS_ARENA_CONNECTIONHANDLER_HH
