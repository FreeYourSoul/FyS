// MIT License
//
// Copyright (c) 2019 Quentin Balland
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


#include <boost/property_tree/ini_parser.hpp>
#include <spdlog/spdlog.h>
#include <tclap/ValueArg.h>
#include <tclap/ValueArg.h>
#include <tclap/CmdLine.h>
#include "../include/ServiceContextBase.hh"

namespace fys::common {

ServiceContextBase::ServiceContextBase(int ac, const char *const *av) try {
    _version = "0.0.1";
    TCLAP::CmdLine cli("FyS::Dispatcher", ' ', _version);
    TCLAP::ValueArg<std::string> configPath("c", "config", "Path of config file", true, "NONE", "string");
    TCLAP::ValueArg<std::string> configSpecificPath("s", "specific_config", "Path of specific config file", true, "NONE", "string");
    TCLAP::ValueArg<std::string> dispatcherHost("a", "dispatcher_address", "Hostname of the service dispatcher", false, "", "string");
    TCLAP::ValueArg<ushort> subport("p", "subport", "Port number of the dispatcher to subscribe to", false, 0, "integer");
    TCLAP::ValueArg<ushort> dispatcherPort("d", "dispatcher_port", "Port number of the service dispatcher", false, 0, "integer");
    cli.add(configPath);
    cli.add(configSpecificPath);
    cli.add(dispatcherHost);
    cli.add(subport);
    cli.add(dispatcherPort);
    cli.parse(ac, av);
    _configFile = configSpecificPath.getValue();
    this->initializeFromIni(configPath.getValue());
    if (dispatcherPort.getValue() > 0)
        _dispatcherData.port = dispatcherPort.getValue();
    if ("NONE" == dispatcherHost.getValue())
        _dispatcherData.address = dispatcherHost.getValue();
    if (subport.getValue() > 0)
        _dispatcherData.subscriberPort = subport.getValue();
}
catch (std::exception &e) {
    SPDLOG_ERROR("Context of the service not initialized caused by : {}", e.what());
}

void ServiceContextBase::initializeFromIni(const std::string &configFilePath) {
    boost::property_tree::ptree pt;
    boost::property_tree::read_ini(configFilePath, pt);

    _name = pt.get<std::string>(fys::common::init_beacon::SERVICE_NAME);
    _hostname = pt.get<std::string>(fys::common::init_beacon::HOSTNAME);
    _port = pt.get<ushort>(fys::common::init_beacon::PORT);
    _dispatcherData.subscriberPort = pt.get<ushort>(fys::common::init_beacon::DISPATCHER_SUBPORT);
    _dispatcherData.port = pt.get<ushort>(fys::common::init_beacon::DISPATCHER_PORT);
    _dispatcherData.address = pt.get<std::string>(fys::common::init_beacon::DISPATCHER_ADDR);

}

std::string ServiceContextBase::getConnectionString() const {
    return std::string("tcp://").append(_hostname).append(":").append(std::to_string(_port));
}

}