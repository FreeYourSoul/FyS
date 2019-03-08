#ifndef FYS_DISPATCHER_HH_
#define FYS_DISPATCHER_HH_

#include <DispatcherConnectionManager.hh>
#include <zmq_addon.hpp>

namespace fys
{
    class StartupDispatcherCtx;

    /**
     * basic implementation of the dispatcher
     */
    class DispatcherHandlerBase {
    protected:
        /**
         * @brief This method is processing the inputMessage and dispatch it appropriately among the peers connected to
         * the DEALER socket
         */
        void processInputMessage();

        /**
         * @brief This method is dispatching the message
         */
        void processClusterMessage();
    };

    /**
     * Main class of the dispatcher
     *
     * @tparam DispatcherHandler class containing two protected method :
     *  processInputMessage()   : Used to dispatch message from the listener socket (basic one)
     *  processClusterMessage() : Used to dispatch message from the subscriber socket (in case of ClusterAware)
     *
     */
    template <typename DispatcherHandler = DispatcherHandlerBase>
    class Dispatcher : private DispatcherHandler {
    public:
        Dispatcher(fys::StartupDispatcherCtx &&ctx)  :
            _connectionManager(1) {
            _connectionManager.setupConnectionManager(ctx);
        }

        void runDispatching() {
            while (true) {
                auto [listenerPolling, subPolling] = _connectionManager.poll();
                if (listenerPolling) {
                    DispatcherHandler::processInputMessage();
                }
                else if (subPolling) {
                    DispatcherHandler::processClusterMessage();
                }
            }

        }

    private:
        fys::network::DispatcherConnectionManager _connectionManager;
    };
} 
#endif // !FYS_DISPATCHER_HH_
