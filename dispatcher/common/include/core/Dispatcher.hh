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
#include <DispatcherConnectionManager.hh>
#include <StartupDispatcherCtx.hh>

// forward declarations
namespace zmq {
    class multipart_t;
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
        void processInputMessage(zmq::multipart_t &&msg, network::DispatcherConnectionManager &manager) noexcept;

        /**
         * @brief This method is dispatching the cluster message and forward it appropriately among the peers connected to
         * the dispatcher socket
         */
        void processClusterMessage(zmq::multipart_t &&msg, network::DispatcherConnectionManager &manager) noexcept;

    protected:
        bool checkAuthentication(const zmq::multipart_t &msg, network::DispatcherConnectionManager &manager) noexcept;

    private:
        std::unordered_set<std::string> _authenticated;
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
            _connectionManager(1, ctx.isLoadBalancingEnabled()) {
            _connectionManager.setupConnectionManager(ctx);
        }


        void runDispatching() {
            while (true) {
                auto [listenerSocketHasSomethingToPoll, subscriberSocketHasSomethingToPoll] = _connectionManager.poll();
                if (listenerSocketHasSomethingToPoll) {
                    _connectionManager.dispatchMessageOnListenerSocket(
                            [this](zmq::multipart_t && msg, network::DispatcherConnectionManager &manager){
                                _dispatcher.processInputMessage(std::move(msg), manager);
                            }
                        );
                }
                else if (subscriberSocketHasSomethingToPoll) {
                    _connectionManager.dispatchMessageOnSubscriberSocket(
                            [this](zmq::multipart_t && msg, network::DispatcherConnectionManager &manager){
                                _dispatcher.processClusterMessage(std::move(msg), manager);
                            }
                        );
                }
            }
        }

        /**
         * Check if the player is currently authenticated:
         *  first check the dispatcher cache
         *
         *  if the dispatcher doesn't contains information for this player, call the authentication server and get the
         *  token for the player
         *      (if authenticated): add this player identifier/token in the dispatcher cache
         *
         * @return a true if the player is authenticated, false otherwise
         */
        constexpr bool checkAuthentication(const std::string &idt, const std::string &token);

    private:
        fys::network::DispatcherConnectionManager _connectionManager;
        DispatcherHandler _dispatcher;
};
} 
#endif // !FYS_DISPATCHER_HH_
