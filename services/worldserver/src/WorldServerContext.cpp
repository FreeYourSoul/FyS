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


#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include "WorldServerContext.hh"

using json = nlohmann::json;

namespace fys::ws {

    WorldServerContext::WorldServerContext(int ac, const char *const *av) :
        common::ServiceContextBase(ac, av) {
        std::ifstream i(_configFile);
        json jsonConfig;
        i >> jsonConfig;
        initWsContextWithJson(jsonConfig);
    }

    void WorldServerContext::initWsContextWithJson(json &json) {
        auto wsJson = json["worldServer"];
        auto confJson = wsJson["conf"];
        auto overlapsJson = confJson["overlapServer"];

        wsJson["code"].get_to(_serverCode);
        _serverXBoundaries = std::make_pair(confJson["begin_x"].get<double>(), confJson["end_x"].get<double>());
        _serverYBoundaries = std::make_pair(confJson["begin_y"].get<double>(), confJson["end_y"].get<double>());
        for (auto &[key, value] : overlapsJson.items()) {
            ProximityServer proximityServer;
            proximityServer.code = value["code"].get<std::string>();

            if (auto proxiXJson = value["overlap_x"]; !proxiXJson.is_null()) {
                proximityServer.xAxisRequirement = {
                        proxiXJson["value"].get<double>(),
                        proxiXJson["condition"].get<std::string>().find(">") != std::string::npos
                };
            }
            if (auto proxiYJson = value["overlap_y"]; !proxiYJson.is_null()) {
                proximityServer.yAxisRequirement = {
                        proxiYJson["value"].get<double>(),
                        proxiYJson["condition"].get<std::string>().find(">") != std::string::npos
                };
            }
            _serverProximity.emplace_back(std::move(proximityServer));
        }
    }

    std::string WorldServerContext::toString() const noexcept {
        std::string str;
        str = "\n*************************\n";
        str+= "[INFO] Service " + _name + " context VERSION: " + _version + "\n";
        str+= "[INFO] Config file used: " + _configFile + "\n\n";
        str+= "[INFO] World Server code: " + _serverCode + "\n";
        str+= "[INFO] Dispatcher subscribing port: " + std::to_string(_dispatcherData.subscriberPort) + "\n";
        str+= "[INFO] Dispatcher connected port: " + std::to_string(_dispatcherData.port) + "\n";
        str+= "[INFO] Dispatcher connected host: " + _dispatcherData.address + "\n";
        for (const auto &prox : _serverProximity) {
            str+= "[INFO] element:\n";
            str+= "code: " + prox.code + "\n";
            if (prox.xAxisRequirement) {
                str+= "Xvalue: " + std::to_string(prox.xAxisRequirement->value) + "\n";
                str+= "XisSup: " + std::to_string(prox.xAxisRequirement->superiorTo) + "\n";
            }
            if (prox.yAxisRequirement) {
                str+= "Yvalue: " + std::to_string(prox.yAxisRequirement->value) + "\n";
                str+= "YisSup: " + std::to_string(prox.yAxisRequirement->superiorTo) + "\n";
            }
        }
        str+= "\n*************************\n";
        return str;
    }

    const std::vector<ProximityServer> &WorldServerContext::getServerProximity() const noexcept {
        return _serverProximity;
    }

    const std::pair<double, double> &WorldServerContext::getServerXBoundaries() const noexcept {
        return _serverXBoundaries;
    }

    const std::pair<double, double> &WorldServerContext::getServerYBoundaries() const noexcept {
        return _serverXBoundaries;
    }

    std::string WorldServerContext::getDispatcherConnectionString() const noexcept {
        return std::string("tcp://").append(_dispatcherData.address).append(":").append(std::to_string(_dispatcherData.port));
    }

    std::string WorldServerContext::getDispatcherSubConnectionString() const noexcept {
        return std::string("tcp://").append(_dispatcherData.address).append(":").append(std::to_string(_dispatcherData.subscriberPort));
    }

}