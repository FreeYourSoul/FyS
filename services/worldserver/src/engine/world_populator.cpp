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

#include <fstream>

#include <CmlKey.hh>
#include <engine/engine.hh>
#include <engine/script_engine.hh>
#include <engine/world_populator.hh>
#include <world_server_context.hh>

namespace {
void assert_engine_error(bool isError, const std::string& errorMsg) {
  if (isError) {
	std::string err = std::string("Miss constructed WorldServerEngine : ").append(errorMsg);
	SPDLOG_ERROR(err);
	throw std::runtime_error(err);
  }
}
}// namespace

namespace fys::ws {

world_populator::~world_populator() = default;

std::shared_ptr<engine>
world_populator::build_world_server_engine() {
  SPDLOG_INFO("[INIT] Start building ServerEngine...");

  ::assert_engine_error(_connection_string.empty(), "Connection string is empty");
  ::assert_engine_error(static_cast<bool>(_script_engine) == false, "Script engine not initialized");
  ::assert_engine_error(static_cast<bool>(_map) == false, "Map is not initialized");

  SPDLOG_INFO("[INIT] ServerEngine setup is correct...");

  auto ret = std::make_shared<engine>(_connection_string,
									  std::move(*_map.get()),
									  std::move(_script_engine),
									  _interval_movement);

  SPDLOG_INFO("[INIT] ServerEngine building is complete");

  return ret;
}

world_populator&
world_populator::populate_script_engine(const world_server_context& ctx) {
  _script_engine = std::make_unique<script_engine>();
  register_common_lua_engine(ctx.path_to_lua_engine());
  generate_spawning_points(ctx.spawning_config_path(), ctx.path_lua_base());
  return *this;
}

world_populator&
world_populator::populate_map(const world_server_context& ctx) {
  // todo : define map and implement it
  return *this;
}

world_populator&
world_populator::set_connection_string(std::string connectionString) {
  _connection_string = std::move(connectionString);
  return *this;
}

world_populator&
world_populator::set_interval_movement(std::chrono::system_clock::duration interval_movement) {
  _interval_movement = interval_movement;
  return *this;
}

}// namespace fys::ws
