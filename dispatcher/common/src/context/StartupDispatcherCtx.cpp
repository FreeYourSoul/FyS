#include <spdlog/spdlog.h>
#include <tclap/ValueArg.h>
#include <tclap/CmdLine.h>
#include <boost/property_tree/ini_parser.hpp>
#include <StartupDispatcherCtx.hh>
#include <Versioner.hh>

namespace fys {

StartupDispatcherCtx::StartupDispatcherCtx(int ac, const char *const *av) noexcept try {
    _version = std::to_string(VERSION_MAJOR) + "." + std::to_string(VERSION_MINOR);
    TCLAP::CmdLine cli("FyS::Dispatcher", ' ', _version);
    TCLAP::ValueArg<std::string> configPath("c", "config", "Path of config file", true, "NONE", "string");
    TCLAP::ValueArg<std::string> configSpecificPath("s", "specificConfig", "Path of specific config file", false, "NONE", "string");
    TCLAP::ValueArg<std::string> name("n", "name", "Name of the Dispatcher (used as key for the cluster)", false, "", "string");
    TCLAP::ValueArg<ushort> changePort("p", "port", "Listening Port", false, 0, "integer");
    TCLAP::ValueArg<bool> aware("a", "aware", "Is aware of the other cluster member", false, true, "boolean");
    TCLAP::ValueArg<bool> loadBalance("l", "load_balance", "Dispatch in load balancing mode or publisher mode", false, true, "boolean");
    TCLAP::ValueArg<bool> verbose("v", "verbose", "Print logs on standard output", false, true, "boolean");

    cli.add(configPath);
    cli.add(changePort);
    cli.add(name);
    cli.add(aware);
    cli.add(verbose);
    cli.add(configSpecificPath);
    cli.parse(ac, av);
    if ("NONE" != configPath.getValue())
        this->initializeFromIni(configPath.getValue());
    if ("NONE" != configSpecificPath.getValue())
        _specificConfigPath = configSpecificPath.getValue();
    if (changePort.getValue() > 0)
        _bindingPort = changePort.getValue();
    if (!name.getValue().empty())
        _name = name.getValue();
    _isClusterAware = !aware.getValue() ? false : _isClusterAware;
    _isLoadBalancing = loadBalance.getValue();
    _verbose = verbose.getValue();
}
catch (std::exception &e) {
    spdlog::get("c")->error("\"Context of the Dispatcher not initialized caused by : {}", e.what());
}


void StartupDispatcherCtx::initializeFromIni(const std::string &configFilePath) {
    boost::property_tree::ptree pt;
    boost::property_tree::read_ini(configFilePath, pt);

    _clusterProxy.backendAddress = pt.get<std::string>(fys::init_beacon::BACKEND_ADDR);
    _clusterProxy.backendPort= pt.get<ushort>(fys::init_beacon::BACKEND_PORT);
    _clusterProxy.frontendAddress = pt.get<std::string>(fys::init_beacon::FRONTEND_ADDR);
    _clusterProxy.frontendPort = pt.get<ushort>(fys::init_beacon::FRONTEND_PORT);
    _bindingPort = pt.get<ushort>(fys::init_beacon::BINDINGPORT);
    _dispatchingPort = pt.get<ushort>(fys::init_beacon::DISPATCHERPORT);
    _isClusterAware = pt.get<bool>(fys::init_beacon::ISCLUSTERAWARE);
    _isLoadBalancing = pt.get<bool>(fys::init_beacon::ISLOADBALANCING);
    _specificConfigPath = pt.get<std::string>(fys::init_beacon::SPECIFIC_CONFIG);
    _name = pt.get<std::string>(fys::init_beacon::NAME);
    std::vector<std::string> topicGroups = parseToArray<std::string>(pt.get<std::string>(fys::init_beacon::TOPIC_GROUPS));
    std::vector<std::string> topics = parseToArray<std::string>(pt.get<std::string>(fys::init_beacon::TOPICS));
    if (topicGroups.empty())
        _subTopics = std::move(topics);
    else {
        _subTopics.reserve(topics.size());
        for (std::string &topic : topics) {
            for (std::string &group : topicGroups) {
                _subTopics.emplace_back(std::string(group + "_").append(topic));
            }
        }
    }
}

std::string StartupDispatcherCtx::toString() const noexcept {
    std::string str;
    str = "\n*************************\n";
    str+= "[INFO] Dispatcher " + _name + " context VERSION: " + _version + "\n\n";
    str+= "[INFO] Client Listener bindingPort: " + std::to_string(_bindingPort) + "\n";
    str+= "[INFO] Service Dispatching Port: " + std::to_string(_dispatchingPort) + "\n";
    str+= "[INFO] isClusterAware: " + std::string(_isClusterAware ? "true" : "false") + "\n";
    str+= "[INFO] isLoadBalancing: " + std::string(_isLoadBalancing ? "true" : "false") + "\n";
    for (const auto &topic : _subTopics) {
        str+= "[INFO] Cluster: Subscribing topic: " + topic + "\n";
    }
    str+= "[INFO] Frontend connection string: " + getFrontendClusterProxyConnectionString() + "\n";
    str+= "[INFO] Backend connection string: " + getBackendClusterProxyConnectionString() + "\n";
    str+= "[INFO] Specific configuration file: " + _specificConfigPath + "\n";
    str+= "\n*************************\n";
    return str;
}

std::string StartupDispatcherCtx::getFrontendClusterProxyConnectionString() const noexcept {
    return "tcp://" + _clusterProxy.frontendAddress + ":" + std::to_string(_clusterProxy.frontendPort);
}

std::string StartupDispatcherCtx::getBackendClusterProxyConnectionString() const noexcept {
    return "tcp://" + _clusterProxy.backendAddress + ":" + std::to_string(_clusterProxy.backendPort);
}

}