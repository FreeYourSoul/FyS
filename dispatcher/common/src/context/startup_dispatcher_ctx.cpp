#include <numeric>
#include <spdlog/spdlog.h>

#include <boost/property_tree/ini_parser.hpp>

#include <fil/cli/command_line_interface.hh>

#include <Versioner.hh>
#include <startup_dispatcher_ctx.hh>

namespace fys {

startup_dispatcher_ctx::startup_dispatcher_ctx(int ac, const char* const* av) noexcept try {
  fil::command_line_interface cli([] {}, "FyS::Dispatcher");
  std::string config_path = "NONE";

  cli.add_option(fil::option(
	  "-c", [&v = config_path](std::string str) { v = std::move(str); }, "Path of the config file"));
  cli.add_option(fil::option(
	  "-s", [&v = _specificConfigPath](std::string str) { v = std::move(str); }, "Path of specific config file"));
  cli.add_option(fil::option(
	  "-n", [&v = _name](std::string str) { v = std::move(str); }, "Name of the Dispatcher (used as key for the cluster)"));
  cli.add_option(fil::option(
	  "-p", [&v = _bindingPort](std::uint64_t value) { v = value; }, "Listening Port"));
  cli.add_option(fil::option(
	  "-a", [&v = _isClusterAware](bool value) { v = value; }, "Is aware of the other cluster member"));
  cli.add_option(fil::option(
	  "-l", [&v = _isLoadBalancing](bool value) { v = value; }, "Dispatch in load balancing mode or publisher mode"));
  cli.add_option(fil::option(
	  "-v", [&v = _verbose](bool value) { v = value; }, "Print logs on standard output"));

  if ("NONE" != config_path)
	this->initialize_from_ini(config_path);
} catch (std::exception& e) {
  SPDLOG_ERROR("\"Context of the Dispatcher not initialized caused by : {}", e.what());
}

void startup_dispatcher_ctx::initialize_from_ini(const std::string& config_file_path) {
  boost::property_tree::ptree pt;
  boost::property_tree::read_ini(config_file_path, pt);

  _maxMsgSize = pt.get<uint>(fys::init_beacon::MAXMSGSIZE);
  _clusterProxy.backend_address = pt.get<std::string>(fys::init_beacon::BACKEND_ADDR);
  _clusterProxy.backend_port = pt.get<ushort>(fys::init_beacon::BACKEND_PORT);
  _clusterProxy.frontend_address = pt.get<std::string>(fys::init_beacon::FRONTEND_ADDR);
  _clusterProxy.frontend_port = pt.get<ushort>(fys::init_beacon::FRONTEND_PORT);

  _bindingPort = pt.get<ushort>(fys::init_beacon::BINDINGPORT);
  _dispatchingPort = pt.get<ushort>(fys::init_beacon::DISPATCHERPORT);
  _isClusterAware = pt.get<bool>(fys::init_beacon::ISCLUSTERAWARE);
  _isLoadBalancing = pt.get<bool>(fys::init_beacon::ISLOADBALANCING);
  _specificConfigPath = pt.get<std::string>(fys::init_beacon::SPECIFIC_CONFIG);
  _name = pt.get<std::string>(fys::init_beacon::NAME);
  std::vector<std::string> topicGroups = to_array<std::string>(pt.get<std::string>(fys::init_beacon::TOPIC_GROUPS));
  std::vector<std::string> topics = to_array<std::string>(pt.get<std::string>(fys::init_beacon::TOPICS));
  if (topicGroups.empty())
	_subTopics = std::move(topics);
  else {
	_subTopics.reserve(topics.size());
	for (std::string& topic : topics) {
	  for (std::string& group : topicGroups) {
		_subTopics.emplace_back(std::string(group + "_").append(topic));
	  }
	}
  }
}

std::string startup_dispatcher_ctx::to_string() const noexcept {
  std::string str;
  str = "\n*************************\n";
  str += "[INFO] Dispatcher " + _name + " context VERSION: " + _version + "\n\n";
  str += "[INFO] Client Listener bindingPort: " + std::to_string(_bindingPort) + "\n";
  str += "[INFO] Listener connection string: " + listener_binding_str() + "\n";
  str += "[INFO] Service Dispatching Port: " + std::to_string(_dispatchingPort) + "\n";
  str += "[INFO] Dispatcher connection string: " + dispatcher_binding_str() + "\n";
  str += "[INFO] isClusterAware: " + std::string(_isClusterAware ? "true" : "false") + "\n";
  str += "[INFO] isLoadBalancing: " + std::string(_isLoadBalancing ? "true" : "false") + "\n";
  str += std::accumulate(_subTopics.begin(), _subTopics.end(), std::string{}, [](std::string a, std::string b) {
	return std::move(a) + "[INFO] Cluster: Subscribing topic: " + std::move(b) + "\n";
  });
  str += "[INFO] Frontend connection string: " + frontend_cluster_proxy_connection_str() + "\n";
  str += "[INFO] Backend connection string: " + backend_cluster_proxy_connection_str() + "\n";
  str += "[INFO] Specific configuration file: " + _specificConfigPath + "\n";
  str += "\n*************************\n";
  return str;
}

std::string startup_dispatcher_ctx::listener_binding_str() const noexcept {
  return "tcp://*:" + std::to_string(_bindingPort);
}

std::string startup_dispatcher_ctx::dispatcher_binding_str() const noexcept {
  return "tcp://*:" + std::to_string(_dispatchingPort);
}

std::string startup_dispatcher_ctx::frontend_cluster_proxy_connection_str() const noexcept {
  return "tcp://" + _clusterProxy.frontend_address + ":" + std::to_string(_clusterProxy.frontend_port);
}

std::string startup_dispatcher_ctx::backend_cluster_proxy_connection_str() const noexcept {
  return "tcp://" + _clusterProxy.backend_address + ":" + std::to_string(_clusterProxy.backend_port);
}

}// namespace fys