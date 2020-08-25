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

#include <unordered_set>
#include <dispatcher_connection_manager.hh>
#include <startup_dispatcher_ctx.hh>

// forward declarations
namespace zmq {
    class multipart_t;
}

namespace fys
{

    /**
     * basic implementation of the dispatcher
     */
    class dispatcher_handler_base {
        static constexpr unsigned CACHE_VALIDITY = 600; // in seconds

    public:
        /**
         * @brief This method is processing the inputMessage and dispatch it appropriately among the peers connected to
         * the dispatcher socket
         */
        void process_input_message(zmq::multipart_t &&msg, network::dispatcher_connection_manager &manager) noexcept;

        /**
         * @brief This method is dispatching the cluster message and forward it appropriately among the peers connected to
         * the dispatcher socket
         */
        void process_cluster_message(zmq::multipart_t &&msg, network::dispatcher_connection_manager &manager) noexcept;

        void forward_message_to_frontend(zmq::multipart_t && msg, network::dispatcher_connection_manager & manager) noexcept;


    private:
        // map of token over timestamp, the timestamp representing the last time the token has been checked
        std::unordered_map<std::string, unsigned> _authenticated;
    };

    /**
     * Main class of the dispatcher
     *
     * @tparam DispatcherHandler class containing two public static method :
     *  processInputMessage()   : Used to dispatch message from the listener socket (basic one)
     *  processClusterMessage() : Used to dispatch message from the subscriber socket (in case of ClusterAware)
     *
     */

    template <typename DispatcherHandler = dispatcher_handler_base>
    class dispatcher {
    public:
        explicit dispatcher(fys::startup_dispatcher_ctx &&ctx)  :
            _connectionManager(1, ctx.isLoadBalancingEnabled()) {
            _connectionManager.setupConnectionManager(ctx);
        }


        void runDispatching() {
            while (true) {
                auto [listenerSocketHasSomethingToPoll,
                      subscriberSocketHasSomethingToPoll,
                      dispatcherSocketHasSomethingToPoll] = _connectionManager.poll();
                if (listenerSocketHasSomethingToPoll) {
                    _connectionManager.dispatchMessageFromListenerSocket(
                            [this](zmq::multipart_t &&msg, network::dispatcher_connection_manager &manager) {
                                _dispatcher.processInputMessage(std::move(msg), manager);
                            }
                    );
                }
                if (subscriberSocketHasSomethingToPoll) {
                    _connectionManager.dispatchMessageFromSubscriberSocket(
                            [this](zmq::multipart_t &&msg, network::dispatcher_connection_manager &manager) {
                                _dispatcher.processClusterMessage(std::move(msg), manager);
                            }
                    );
                }
                if (dispatcherSocketHasSomethingToPoll) {
                    _connectionManager.dispatchMessageFromDispatcherSocket(
                            [this](zmq::multipart_t && msg, network::dispatcher_connection_manager &manager){
                                _dispatcher.forwardMessageToFrontend(std::move(msg), manager);
                            }
                        );
                }
            }
        }

    private:
        fys::network::dispatcher_connection_manager _connectionManager;
        DispatcherHandler _dispatcher;
};
} 
#endif // !FYS_DISPATCHER_HH_
