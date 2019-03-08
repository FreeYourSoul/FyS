#ifndef FYS_CONNECTIONMANAGER_HH_
#define FYS_CONNECTIONMANAGER_HH_

#include <zmq.hpp>

namespace fys {
    class StartupDispatcherCtx;
}

namespace fys::network {

    struct ClusterConnection {
        zmq::socket_t subSocket; // Connect on back end of the proxy
        zmq::socket_t pubSocket; // Connect on front end of the proxy
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
            explicit DispatcherConnectionManager(int threadNumber = 1,  bool isLoadBalancing = true);

            void setupConnectionManager(const fys::StartupDispatcherCtx &ctx);

        private:
            /**
             * The dispatcher connect to the proxy and subscribe to specifics channels given as parameter
             * @param topics additional topics to subscribe to (customizable through the config file)
             */
            void subscribeToTopics(const std::vector<std::string> &topics);
            
        private:
            zmq::context_t _zmqContext;
            zmq::socket_t _listener;
            zmq::socket_t _dipatcher;
            ClusterConnection _clusterConnection;

    };
    
}

#endif // !FYS_CONNECTIONMANAGER_HH_
