#include <spdlog/spdlog.h>
#include <tclap/ValueArg.h>
#include <tclap/CmdLine.h>
#include <boost/property_tree/ini_parser.hpp>
#include <StartupDispatcherCtx.hh>
#include <Versioner.hh>

fys::StartupDispatcherCtx::StartupDispatcherCtx(int ac, const char *const *av) try {
    _version = std::to_string(VERSION_MAJOR) + "." + std::to_string(VERSION_MINOR);
    TCLAP::CmdLine cli("FyS::Dispatcher", ' ', _version);
    TCLAP::ValueArg<std::string> configPath("c", "config", "Path of config file", false, "NONE", "string");
    TCLAP::ValueArg<std::string> name("n", "name", "Name of the Dispatcher (used as key for the cluster)", false, "WS", "string");
    TCLAP::ValueArg<ushort> changePort("p", "port", "Listening Port", false, 0, "integer");
    TCLAP::ValueArg<bool> aware("a", "aware", "Is aware of the other cluster member", false, true, "boolean");
    TCLAP::ValueArg<bool> loadBalance("l", "load_balance", "Dispatch in load balancing mode or publisher mode", false, true, "boolean");
    TCLAP::ValueArg<bool> verbose("v", "verbose", "Print logs on standard output", false, true, "boolean");

    cli.add(configPath);
    cli.add(changePort);
    cli.add(name);
    cli.add(aware);
    cli.add(verbose);
    cli.parse(ac, av);
    if ("NONE" != configPath.getValue())
        this->initializeFromIni(configPath.getValue());
    if (_bindingPort > 0)
        _bindingPort = changePort.getValue();
    if (_name.empty())
        _name = name.getValue();
    setIsClusterAware(!aware.getValue() ? false : _isClusterAware);
    _isLoadBalancing = loadBalance.getValue();
    _verbose = verbose.getValue();
}
catch (std::exception &e) {
    spdlog::get("c")->error("\"Context of the Dispatcher not initialized caused by : {}", e.what());
}


void fys::StartupDispatcherCtx::initializeFromIni(const std::string &configFilePath) {
    boost::property_tree::ptree pt;
    boost::property_tree::read_ini(configFilePath, pt);

    _bindingPort = pt.get<ushort>(fys::INIT_BINDINGPORT);
    _isClusterAware = pt.get<bool>(fys::INIT_ISCLUSTERAWARE);
    _isLoadBalancing = pt.get<bool>(fys::INIT_ISLOADBALANCING);
    _name = pt.get<std::string>(fys::INIT_NAME);
    _subTopics = asVector<std::string>(pt, fys::INIT_TOPICS);
}
