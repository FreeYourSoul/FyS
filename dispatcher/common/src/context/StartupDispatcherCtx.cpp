#include <iostream>
#include <tclap/ValueArg.h>
#include <tclap/CmdLine.h>
#include <StartupDispatcherCtx.hh>

fys::StartupDispatcherCtx::StartupDispatcherCtx(const int ac, const char *const *av) try {
    TCLAP::CmdLine cli("FyS::Dispatcher", ' ', VERSION_DISPATCHER);
    TCLAP::ValueArg<std::string> configPath("c", "config", "Path of config file", false, "NONE", "string");
    TCLAP::ValueArg<std::string> name("n", "name", "Name of the Dispatcher (used as key for the cluster)", false, "WS", "string");
    TCLAP::ValueArg<ushort> changePort("p", "port", "Listening Port", false, 0, "integer");
    TCLAP::ValueArg<bool> aware("a", "aware", "Is aware of the other cluster member", false, true, "boolean");
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
    if (_name.empty)
        _name = name.getValue();
    _isClusterAware = !aware.getValue() ? false : _isClusterAware;
    _verbose = verbose.getValue();
}
catch (std::exception &e) {
    std::cerr << "Context of the Dispatcher not initialized caused by :" << e.what() << "\n";
}


fys::StartupDispatcherCtx::initializeFromIni(const std::string &configFilePath) {
    boost::property_tree::ptree pt;
    boost::property_tree::read_ini(iniPath, pt);

    _bindingPort = pt.get<ushort>(fys::INIT_BINDINGPORT);
    _isClusterAware = pt.get<bool>(fys::INIT_ISCLUSTERAWARE);
    _name = pt.get<std::string>(fys::INIT_NAME);
    _subTopics = asVector<std::string>(pt, fys::INIT_TOPICS);
}
