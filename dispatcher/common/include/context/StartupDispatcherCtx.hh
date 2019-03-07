#ifndef FYS_STARTUP_DISPATCHER_HH_
#define FYS_STARTUP_DISPATCHER_HH_

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>

#ifndef VERSION_DISPATCHER
 #define VERSION_DISPATCHER "1.0"
#endif

namespace fys {

    constexpr auto INIT_NAME            = "name";
    constexpr auto INIT_TOPICS          = "topics";
    constexpr auto INIT_ISCLUSTERAWARE  = "isClusterAware";
    constexpr auto INIT_BINDINGPORT     = "bindingPort";

    class StartupDispatcherCtx {
        public:
            StartupDispatcherCtx(const int ac, const char *const *av);

            template <typename T>
            void setName(T && name) {
                _name = std::forward(name);
            }
            void setIsClusterAware(bool isClusterAware) { _isClusterAware = isClusterAware; }

            bool isClusterAware() const { return _isClusterAware; }
            bool isVerbose() const { return _verbose; }
            ushort getBindingPort() const { return _bindingPort; }
            const std::string &getName() const { return _name; }
            const std::vector<std::string> getSubscriptionTopics() const { return _subTopics; }

        private:
            void initializeFromIni(const std::string &configFilePath);

            template <typename T>
            std::vector<T> asVector(boost::property_tree::ptree const& pt, boost::property_tree::ptree::key_type const& key)
            {
                std::vector<T> r;
                for (auto& item : pt.get_child(key))
                    r.push_back(item.second.get_value<T>());
                return r;
            }

        private:
            std::string _name;
            ushort _bindingPort = 0;
            bool _verbose = false;
            bool _isClusterAware = false;
            std::vector<std::string> _subTopics;

    };

}

#endif /* FYS_STARTUP_DISPATCHER_HH_ */
