// MIT License
//
// Copyright (c) 2021-2022 Quentin Balland
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

#ifndef FYS_STARTUP_DISPATCHER_HH_
#define FYS_STARTUP_DISPATCHER_HH_

#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>

#ifndef VERSION_DISPATCHER
#define VERSION_DISPATCHER "1.0"
#endif

namespace fys {

namespace init_beacon {
constexpr auto NAME = "dispatcher.name";
constexpr auto TOPICS = "dispatcher.topics";
constexpr auto ISCLUSTERAWARE = "dispatcher.isClusterAware";
constexpr auto MAXMSGSIZE = "dispatcher.maxMsgSize";
constexpr auto BINDINGPORT = "dispatcher.bindingPort";
constexpr auto DISPATCHERPORT = "dispatcher.dispatcherPort";
constexpr auto ISLOADBALANCING = "dispatcher.isLoadBalancing";
constexpr auto TOPIC_GROUPS = "dispatcher.topicGroups";
constexpr auto SPECIFIC_CONFIG = "dispatcher.specific_config";
constexpr auto FRONTEND_PORT = "proxy_frontend.port";
constexpr auto FRONTEND_ADDR = "proxy_frontend.address";
constexpr auto BACKEND_PORT = "proxy_backend.port";
constexpr auto BACKEND_ADDR = "proxy_backend.address";
}// namespace init_beacon

class startup_dispatcher_ctx {
  struct ClusterProxy {
    ushort frontend_port = 0;
    ushort backend_port = 0;
    std::string frontend_address;
    std::string backend_address;
  };

public:
  startup_dispatcher_ctx(int ac, const char* const* av) noexcept;

  [[nodiscard]] constexpr bool isLoadBalancingEnabled() const noexcept { return _isLoadBalancing; }
  [[nodiscard]] constexpr bool isClusterAware() const noexcept { return _isClusterAware; }
  [[nodiscard]] constexpr ushort getBindingPort() const noexcept { return _bindingPort; }
  [[nodiscard]] constexpr ushort getDispatchingPort() const noexcept { return _dispatchingPort; }
  [[nodiscard]] constexpr uint getMaxMsgSize() const noexcept { return _maxMsgSize; }

  [[nodiscard]] const std::vector<std::string>& getSubscriptionTopics() const noexcept { return _subTopics; }
  [[nodiscard]] std::string listener_binding_str() const noexcept;
  [[nodiscard]] std::string dispatcher_binding_str() const noexcept;
  [[nodiscard]] std::string frontend_cluster_proxy_connection_str() const noexcept;
  [[nodiscard]] std::string backend_cluster_proxy_connection_str() const noexcept;

  /**
   * @return debug string containing the context
   */
  [[nodiscard]] std::string to_string() const noexcept;

private:
  void initialize_from_ini(const std::string& config_file_path);

  template<typename T>
  [[nodiscard]] std::vector<T> to_array(const std::string& s) const noexcept {
    std::vector<T> result;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, ','))
      result.push_back(boost::lexical_cast<T>(item));
    return result;
  }

protected:
  std::string _version;
  std::string _name;

  /**
   * Client port
   */
  ushort _bindingPort = 0;
  /**
   * Dispatching to service port (for publishing / load balancing)
   */
  ushort _dispatchingPort = 0;

  /**
   * Maximum size in byte of an inbound message
   */
  uint _maxMsgSize = 1024;

  bool _verbose = false;

  ClusterProxy _clusterProxy;
  std::vector<std::string> _subTopics;

  std::string _specificConfigPath;

  bool _isClusterAware = false;
  bool _isLoadBalancing = true;
};

}// namespace fys

#endif /* FYS_STARTUP_DISPATCHER_HH_ */
