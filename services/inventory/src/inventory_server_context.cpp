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

#include "inventory_server_context.hh"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

namespace fys::inv {

inventory_server_context::inventory_server_context(int argc, char **argv)
	: common::service_context_base(argc, argv) {
  std::ifstream i(_config_file);
  json jsonConfig;
  i >> jsonConfig;
  init_inventory_context_with_json(jsonConfig);
}

std::string
inventory_server_context::to_string() const {
  std::string str;
  str = "dump context\n*************************\n";
  str += "[INFO] Service " + _name + " context VERSION: " + _version + "\n";
  str += "[INFO] Config file used: " + _config_file + "\n";
  str += "[INFO] Server code " + _server_code + "\n";
  str += "\n[INFO] Dispatcher connected port: " + std::to_string(_dispatcher_data.port) + "\n";
  str += "[INFO] Dispatcher connected host: " + _dispatcher_data.address + "\n";
  str += "[INFO] Dispatcher connection string: " + dispatcher_connection_str() + "\n";
  str += "[INFO] Player connection string: " + get_player_connection_str() + "\n";
  str += "\n*************************\n";
  return str;
}

std::string
inventory_server_context::get_player_connection_str() const noexcept {
  return std::string("tcp://*:").append(std::to_string(_portPlayerConnection));
}

void inventory_server_context::init_inventory_context_with_json(json &json) {
  auto invJson = json["inventory"];
  invJson["code"].get_to(_server_code);
  _portPlayerConnection = invJson["player_connection_port"].get<uint>();
}

}// namespace fys::inv