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

#ifndef FYS_DISPATCHER_HH_
#define FYS_DISPATCHER_HH_

#include <DispatcherConnectionManager.hh>

// forward declarations
namespace zmq {
    class multipart_t;
}
namespace fys {
    class StartupDispatcherCtx;
}

namespace fys
{

    /**
     * basic implementation of the dispatcher
     */
    class DispatcherHandlerBase {
    public:
        /**
         * @brief This method is processing the inputMessage and dispatch it appropriately among the peers connected to
         * the dispatcher socket
         */
        static void processInputMessage(zmq::multipart_t &&msg, network::DispatcherConnectionManager &manager) noexcept;

        /**
         * @brief This method is dispatching the cluster message and forward it appropriately among the peers connected to
         * the dispatcher socket
         */
        static void processClusterMessage(zmq::multipart_t &&msg, network::DispatcherConnectionManager &manager) noexcept;
    };

    /**
     * Main class of the dispatcher
     *
     * @tparam DispatcherHandler class containing two public static method :
     *  processInputMessage()   : Used to dispatch message from the listener socket (basic one)
     *  processClusterMessage() : Used to dispatch message from the subscriber socket (in case of ClusterAware)
     *
     */
    template <typename DispatcherHandler = DispatcherHandlerBase>
    class Dispatcher {
    public:
        explicit Dispatcher(fys::StartupDispatcherCtx &&ctx)  :
            _connectionManager(1) {
            _connectionManager.setupConnectionManager(ctx);
        }

        void runDispatching() {
            while (true) {
                auto [listenerSocketHasSomethingToPoll, subscriberSocketHasSomethingToPoll] = _connectionManager.poll();
                if (listenerSocketHasSomethingToPoll) {
                    _connectionManager.dispatchMessageOnListenerSocket([](zmq::multipart_t && msg, network::DispatcherConnectionManager &manager){
                        DispatcherHandler::processInputMessage(std::move(msg), manager);
                    });
                }
                else if (subscriberSocketHasSomethingToPoll) {
                    _connectionManager.dispatchMessageOnSubscriberSocket([](zmq::multipart_t && msg, network::DispatcherConnectionManager &manager){
                        DispatcherHandler::processClusterMessage(std::move(msg), manager);
                    });
                }
            }

        }

    private:
        fys::network::DispatcherConnectionManager _connectionManager;
    };
} 
#endif // !FYS_DISPATCHER_HH_
