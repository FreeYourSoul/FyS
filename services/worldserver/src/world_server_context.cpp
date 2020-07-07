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
#include "world_server_context.hh"

using json = nlohmann::json;

namespace fys::ws {

world_server_context::world_server_context(int ac, const char* const* av)
		:
		common::service_context_base(ac, av)
{
	std::ifstream i(_config_file);
	json json_config;
	i >> json_config;
	initWsContextWithJson(json_config);
}

void
world_server_context::initWsContextWithJson(json& json)
{
	auto ws_json = json["worldServer"];
	auto conf_json = ws_json["conf"];
	auto overlap_json = conf_json["overlapServer"];

	ws_json["code"].get_to(_server_code);
	ws_json["connection_port"].get_to(_port_player_connection);
	ws_json["TMX_Map"].get_to(_tmx_map_path);
	ws_json["spawning_config"].get_to(_spawning_config_path);
	ws_json["path_lua_init_engine"].get_to(_path_to_lua_init_engine);
	ws_json["script_storage_cache"].get_to(_path_to_local_storage);
	ws_json["path_lua_base"].get_to(_path_lua_base);
	_server_x_boundaries = {conf_json["begin_x"].get<double>(), conf_json["end_x"].get<double>()};
	_server_y_boundaries = {conf_json["begin_y"].get<double>(), conf_json["end_y"].get<double>()};
	for (auto &[key, value] : overlap_json.items()) {
		proximity_server proximityServer;
		proximityServer.code = value["code"].get<std::string>();

		if (auto proximity_x_json = value["overlap_x"]; !proximity_x_json.is_null()) {
			proximityServer.x_axis_requirement = {
					proximity_x_json["value"].get<double>(),
					proximity_x_json["condition"].get<std::string>().find(">") != std::string::npos
			};
		}
		if (auto proximity_y_json = value["overlap_y"]; !proximity_y_json.is_null()) {
			proximityServer.y_axis_requirement = {
					proximity_y_json["value"].get<double>(),
					proximity_y_json["condition"].get<std::string>().find(">") != std::string::npos
			};
		}
		_server_proximity.emplace_back(std::move(proximityServer));
	}
}

std::string
world_server_context::to_string() const noexcept
{
	std::string str;
	str = "\n*************************\n";
	str += "[INIT] Service " + _name + " context VERSION: " + _version + "\n";
	str += "[INIT] Config file used: " + _config_file + "\n\n";
	str += "[INIT] World Server code: " + _server_code + "\n";
	str += "[INIT] Local CML Storage : " + _path_to_local_storage + "\n";
	str += "[INIT] Path to LUA Engine initiator : " + _path_to_lua_init_engine + "\n";
	str += "[INIT] TMX Map path: " + _tmx_map_path + "\n"; // Will be changed with the new formatting file homemade
	str += "[INIT] Player connection string: " + get_player_connection_str() + "\n";
	str += "[INIT] Dispatcher(AuthServer) subscribing port: " + std::to_string(_dispatcherData.subscriber_port) + "\n";
	str += "[INIT] Dispatcher(AuthServer) connected port: " + std::to_string(_dispatcherData.port) + "\n";
	str += "[INIT] Dispatcher(AuthServer) connected host: " + _dispatcherData.address + "\n";
	str += "[INIT] Dispatcher(AuthServer) Subscriber connection string: " + get_dispatcher_sub_connection_str() + "\n";
	str += "[INIT] Dispatcher(AuthServer) connection string: " + get_dispatcher_connection_str() + "\n";

	for (const auto& prox : _server_proximity) {
		str += "[INIT] element:\n       ";
		str += "code: " + prox.code + "\n";
		if (prox.x_axis_requirement) {
			str += "       Xvalue: " + std::to_string(prox.x_axis_requirement->value) + "\n";
			str += "       XisSup: " + std::to_string(prox.x_axis_requirement->superiorTo) + "\n";
		}
		if (prox.y_axis_requirement) {
			str += "       Yvalue: " + std::to_string(prox.y_axis_requirement->value) + "\n";
			str += "       YisSup: " + std::to_string(prox.y_axis_requirement->superiorTo) + "\n";
		}
	}
	str += "\n*************************";
	return str;
}

std::string
world_server_context::get_dispatcher_connection_str() const noexcept
{
	return std::string("tcp://").append(_dispatcherData.address).append(":").append(std::to_string(_dispatcherData.port));
}

std::string
world_server_context::get_dispatcher_sub_connection_str() const noexcept
{
	return std::string("tcp://").append(_dispatcherData.address).append(":").append(std::to_string(_dispatcherData.subscriber_port));
}

std::string
world_server_context::get_player_connection_str() const noexcept
{
	return std::string("tcp://*:").append(std::to_string(_port_player_connection));
}


}