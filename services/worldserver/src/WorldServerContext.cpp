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

WorldServerContext::WorldServerContext(int ac, const char* const* av)
		:
		common::ServiceContextBase(ac, av)
{
	std::ifstream i(_configFile);
	json jsonConfig;
	i >> jsonConfig;
	initWsContextWithJson(jsonConfig);
}

void
WorldServerContext::initWsContextWithJson(json& json)
{
	auto wsJson = json["worldServer"];
	auto confJson = wsJson["conf"];
	auto overlapsJson = confJson["overlapServer"];

	wsJson["code"].get_to(_serverCode);
	wsJson["connection_port"].get_to(_portPlayerConnection);
	wsJson["TMX_Map"].get_to(_tmxMapPath);
	wsJson["spawning_config"].get_to(_spawningConfigPath);
	wsJson["path_lua_init_engine"].get_to(_pathToLuaInitEngine);
	wsJson["script_storage_cache"].get_to(_pathToLocalStorage);
	wsJson["path_lua_base"].get_to(_pathLuaBase);
	_serverXBoundaries = {confJson["begin_x"].get<double>(), confJson["end_x"].get<double>()};
	_serverYBoundaries = {confJson["begin_y"].get<double>(), confJson["end_y"].get<double>()};
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

std::string
WorldServerContext::toString() const noexcept
{
	std::string str;
	str = "\n*************************\n";
	str += "[INIT] Service " + _name + " context VERSION: " + _version + "\n";
	str += "[INIT] Config file used: " + _configFile + "\n\n";
	str += "[INIT] World Server code: " + _serverCode + "\n";
	str += "[INIT] Local CML Storage : " + _pathToLocalStorage + "\n";
	str += "[INIT] Path to LUA Engine initiator : " + _pathToLuaInitEngine + "\n";
	str += "[INIT] TMX Map path: " + _tmxMapPath + "\n"; // Will be changed with the new formatting file homemade
	str += "[INIT] Player connection string: " + getPlayerConnectionString() + "\n";
	str += "[INIT] Dispatcher(AuthServer) subscribing port: " + std::to_string(_dispatcherData.subscriberPort) + "\n";
	str += "[INIT] Dispatcher(AuthServer) connected port: " + std::to_string(_dispatcherData.port) + "\n";
	str += "[INIT] Dispatcher(AuthServer) connected host: " + _dispatcherData.address + "\n";
	str += "[INIT] Dispatcher(AuthServer) Subscriber connection string: " + getDispatcherSubConnectionString() + "\n";
	str += "[INIT] Dispatcher(AuthServer) connection string: " + getDispatcherConnectionString() + "\n";

	for (const auto& prox : _serverProximity) {
		str += "[INIT] element:\n       ";
		str += "code: " + prox.code + "\n";
		if (prox.xAxisRequirement) {
			str += "       Xvalue: " + std::to_string(prox.xAxisRequirement->value) + "\n";
			str += "       XisSup: " + std::to_string(prox.xAxisRequirement->superiorTo) + "\n";
		}
		if (prox.yAxisRequirement) {
			str += "       Yvalue: " + std::to_string(prox.yAxisRequirement->value) + "\n";
			str += "       YisSup: " + std::to_string(prox.yAxisRequirement->superiorTo) + "\n";
		}
	}
	str += "\n*************************";
	return str;
}

std::string
WorldServerContext::getDispatcherConnectionString() const noexcept
{
	return std::string("tcp://").append(_dispatcherData.address).append(":").append(std::to_string(_dispatcherData.port));
}

std::string
WorldServerContext::getDispatcherSubConnectionString() const noexcept
{
	return std::string("tcp://").append(_dispatcherData.address).append(":").append(std::to_string(_dispatcherData.subscriberPort));
}

std::string
WorldServerContext::getPlayerConnectionString() const noexcept
{
	return std::string("tcp://*:").append(std::to_string(_portPlayerConnection));
}


}