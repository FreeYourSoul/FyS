#ifndef FYS_STARTUP_DISPATCHER_HH_
#define FYS_STARTUP_DISPATCHER_HH_

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <string>

#ifndef VERSION_DISPATCHER
 #define VERSION_DISPATCHER "1.0"
#endif

namespace fys {

    namespace init_beacon {
        constexpr auto NAME            = "dispatcher.name";
        constexpr auto TOPICS          = "dispatcher.topics";
        constexpr auto ISCLUSTERAWARE  = "dispatcher.isClusterAware";
        constexpr auto BINDINGPORT     = "dispatcher.bindingPort";
        constexpr auto ISLOADBALANCING = "dispatcher.isLoadBalancing";
        constexpr auto FRONTEND_PORT   = "proxy_frontend.port";
        constexpr auto FRONTEND_ADDR   = "proxy_frontend.address";
        constexpr auto BACKEND_PORT    = "proxy_backend.port";
        constexpr auto BACKEND_ADDR    = "proxy_backend.address";
    }

    class StartupDispatcherCtx {
        struct ClusterProxy {
            ushort frontendPort = 0;
            ushort backendPort = 0;
            std::string frontendAddress;
            std::string backendAddress;
        };

        public:
            StartupDispatcherCtx(int ac, const char *const *av);

            bool isClusterAware() const { return _isClusterAware; }
            ushort getBindingPort() const { return _bindingPort; }

            const std::vector<std::string> getSubscriptionTopics() const { return _subTopics; }
            std::string getFrontendClusterProxyConnectionString() const;
            std::string getBackendClusterProxyConnectionString () const;


            /**
             * @return debug string containing the context
             */
            std::string toString() const;

        private:
            void initializeFromIni(const std::string &configFilePath);

            template<typename T>
            std::vector<T> parseToArray(const std::string &s)
            {
                std::vector<T> result;
                std::stringstream ss(s);
                std::string item;
                while(std::getline(ss, item, ','))
                    result.push_back(boost::lexical_cast<T>(item));
                return result;
            }

        private:
            std::string _version;
            std::string _name;

            ushort _bindingPort = 0;
            bool _verbose = false;

            ClusterProxy _clusterProxy;
            std::vector<std::string> _subTopics;

            bool _isClusterAware = false;
            bool _isLoadBalancing = true;

    };

}

#endif /* FYS_STARTUP_DISPATCHER_HH_ */
