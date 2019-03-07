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
            DispatcherConnectionManager(int threadNumber, const std::string &proxyFeConnectionString, const std::string &proxyBeConnectionString);

            void setupConnectionManager(const fys::StartupDispatcherCtx &ctx);

        private:
            void subscribeToTopics(const std::vector<std::string> &topics);
            
        private:
            zmq::context_t _zmqContext;
            ClusterConnection _clusterConnection;
            zmq::pollitem_t _items[1];
            
    };
    
}

#endif // !FYS_CONNECTIONMANAGER_HH_
