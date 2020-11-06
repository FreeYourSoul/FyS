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

#ifndef FYS_WORLDSERVERCONTEXT_HH
#define FYS_WORLDSERVERCONTEXT_HH

#include <nlohmann/json.hpp>

#include <engine/collision_map.hh>

#include <service_context_base.hh>

namespace fys::ws {

class world_server_context : fys::common::service_context_base {

public:
  explicit world_server_context(int ac, const char *const *av);

  [[nodiscard]] std::string
  to_string() const noexcept;

  [[nodiscard]] std::string
  dispatcher_sub_connection_str() const noexcept;

  [[nodiscard]] std::string
  dispatcher_connection_str() const noexcept;

  [[nodiscard]] std::string
  player_connection_str() const noexcept;

  [[nodiscard]] const std::string &
  path_to_local_storage() const noexcept { return _path_to_local_storage; }

  [[nodiscard]] const std::string &
  server_code() const noexcept { return _server_code; }

  [[nodiscard]] const std::string &
  tmx_map_path() const noexcept { return _tmx_map_path; }

  [[nodiscard]] const boundary &
  server_x_boundaries() const noexcept { return _server_x_boundaries; }

  [[nodiscard]] const boundary &
  server_y_boundaries() const noexcept { return _server_y_boundaries; }

  [[nodiscard]] const std::vector<proximity_server> &
  server_proximity() const noexcept { return _server_proximity; }

  [[nodiscard]] const std::string &
  path_to_lua_engine() const noexcept { return _path_to_lua_init_engine; }

  [[nodiscard]] const std::string &
  spawning_config_path() const noexcept { return _spawning_config_path; }

  [[nodiscard]] const std::string &
  path_lua_base() const noexcept { return _path_lua_base; }

private:
  void init_ws_ctx_from_json(nlohmann::json &json);

private:
  std::string _server_code;
  std::string _tmx_map_path;
  std::string _path_to_local_storage;
  std::string _path_lua_base;
  std::string _path_to_lua_init_engine;
  std::string _spawning_config_path;

  std::uint32_t _port_player_connection;
  boundary _server_x_boundaries;
  boundary _server_y_boundaries;
  std::vector<proximity_server> _server_proximity;
};

}// namespace fys::ws

#endif//FYS_WORLDSERVERCONTEXT_HH
