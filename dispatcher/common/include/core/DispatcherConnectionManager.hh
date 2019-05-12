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

#ifndef FYS_CONNECTIONMANAGER_HH_
#define FYS_CONNECTIONMANAGER_HH_

#include <spdlog/spdlog.h>
#include <zmq_addon.hpp>

namespace fys {
    class StartupDispatcherCtx;
}

namespace fys::network {

    struct ClusterConnection {
        zmq::socket_t subSocket; // Connect on back end of the proxy
        zmq::socket_t pubSocket; // Connect on front end of the proxy
        bool closed = false;
    };

    class DispatcherConnectionManager {
        public:
        /**
         * @param threadNumber
         *          number of thread used by the zmq::context in order to manage messages
         * @param isLoadBalancing
         *          if true, a Dealer socket is instantiated in order to dispatch messages
         *          if false, a Publisher socket is instantiated to do so
         */
        explicit DispatcherConnectionManager(int threadNumber = 1,  bool isLoadBalancing = true) noexcept;

        void setupConnectionManager(const fys::StartupDispatcherCtx &ctx) noexcept;

        /**
         * @brief Poll the reader socket (the listener one and the subscriber one if the dispatcher is cluster
         * aware
         *
         * @return a pair of bool with
         *         first  : is there anything on the listener socket to read
         *         second : is there anything on the subscriber socket to read
         */
        std::pair<bool, bool> poll() noexcept;


        /**
         * @brief Read the Listener socket and call a given handler with this message as parameter
         *
         * @tparam Handler function to call after reading message from socket (multipart_t object sent as parameter)
         * @param handler function instance to call which take zmq::multipart object as parameter
         */
        template <typename Handler>
        void dispatchMessageOnListenerSocket(Handler &&handler) {
            zmq::multipart_t msg;
            if (!msg.recv(_listener))
                SPDLOG_ERROR("Error while reading on the listener socket");
            else
                handler(std::move(msg), *this);
        }

        /**
         * @brief Read the Cluster subscriber socket and call a given handler with this message as parameter
         *
         * @tparam Handler function to call after reading message from socket (multipart_t object sent as parameter)
         * @param handler function instance to call which take zmq::multipart object as parameter
         */
        template <typename Handler>
        void dispatchMessageOnSubscriberSocket(Handler &&handler) noexcept {
            if (!_clusterConnection.closed) {
                zmq::multipart_t msg;
                if (!msg.recv(_clusterConnection.subSocket))
                    SPDLOG_ERROR("Error while reading on the cluster subscriber socket");
                else
                    handler(std::move(msg), *this);
            }
        }

        bool sendMessageToDispatcherSocket(zmq::multipart_t &&msg) noexcept;
        bool sendMessageToClusterPubSocket(zmq::multipart_t &&msg) noexcept;

    private:
        /**
         * The dispatcher connect to the proxy and subscribe to specifics channels given as parameter
         * @param topics additional topics to subscribe to (customizable through the config file)
         */
        void subscribeToTopics(const std::vector<std::string> &topics) noexcept;
            
        private:
            bool _isLoadBalancing;
            zmq::context_t _zmqContext;
            zmq::socket_t _listener;
            zmq::socket_t _dispatcher;

            ClusterConnection _clusterConnection;

    };
    
}

#endif // !FYS_CONNECTIONMANAGER_HH_
