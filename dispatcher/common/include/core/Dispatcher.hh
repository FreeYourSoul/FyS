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
        static void processInputMessage(zmq::multipart_t &&msg, network::DispatcherConnectionManager &manager);

        /**
         * @brief This method is dispatching the cluster message and forward it appropriately among the peers connected to
         * the dispatcher socket
         */
        static void processClusterMessage(zmq::multipart_t &&msg, network::DispatcherConnectionManager &manager);
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
