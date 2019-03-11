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
        explicit DispatcherConnectionManager(int threadNumber = 1,  bool isLoadBalancing = true);

        void setupConnectionManager(const fys::StartupDispatcherCtx &ctx);

        /**
         * @brief Poll the reader socket (the listener one and the subscriber one if the dispatcher is cluster
         * aware
         *
         * @return a pair of bool with
         *         first  : is there anything on the listener socket to read
         *         second : is there anything on the subscriber socket to read
         */
        std::pair<bool, bool> poll();


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
                spdlog::get("c")->error("Error while reading on the listener socket");
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
        void dispatchMessageOnSubscriberSocket(Handler &&handler) {
            if (!_clusterConnection.closed) {
                zmq::multipart_t msg;
                if (!msg.recv(_clusterConnection.subSocket))
                    spdlog::get("c")->error("Error while reading on the cluster subscriber socket");
                else
                    handler(std::move(msg), *this);
            }
        }

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
