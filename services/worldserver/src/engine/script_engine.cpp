// MIT License
//
// Copyright (c) 2021-2022 Quentin Balland
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

#include <fmt/format.h>

#include <algorithm>
#include <fstream>
#include <memory>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <fmt/format.h>

#include <cml_key.hh>
#include <logger.hh>
#include <world_server_context.hh>

#include <engine/script_engine.hh>
#include <engine/world_populator.hh>

namespace fys::ws {

namespace {

/**
 * @brief Merge actions notification from a given spawning_point (central position, distance of notification),
 *
 * TO THINK ABOUT : Making a merge that decrease the amount of spawning points, The issue being that if
 * a spawning point get merged with another one, the distance of notification has to be enlarged. Otherwise
 * players may not see some encounters...
 *
 * @param report to merge into
 * @param actions reported on the given spawning point (to be merged)
 * @param central_position_actions central point of the spawning point in which the actions occurred
 * @param distance_notif distance notification at which players should be notified
 */
void merge_spawning_point_in_report(npc_actions_report& report) {
}

[[nodiscard]] std::string
get_path_from_key(std::string base, std::string key) {
  std::replace(key.begin(), key.end(), ':', '/');
  return base.append("/").append(std::move(key));
}

[[nodiscard]] std::string
get_sp_namespace_from_key(const std::string& key) {
  auto startSeparator = key.find_last_of(':');
  auto endSeparator = key.find_last_of('.');

  if (startSeparator != std::string::npos && endSeparator != std::string::npos) {
    // if a ':' and a '.' are found
    return key.substr(startSeparator + 1, endSeparator - startSeparator - 1);
  } else if (startSeparator == std::string::npos && endSeparator != std::string::npos) {
    // if a ':' is not found but not a '.' is found
    return key.substr(0, endSeparator);
  } else if (startSeparator != std::string::npos && endSeparator == std::string::npos) {
    // if a ':' is found but not a '.' is not found
    return key.substr(startSeparator + 1);
  }
  return key;
}

}// namespace

/**
 * @brief A representation of a NPC instance (NPC can be a neutral npc or an encounter)
 */
struct npc_lua_instance {
  character_info info;

  //! Spawning Point namespace, empty in case of neutral encounter
  std::string sp_namespace;

  /**
   * @brief Id of the npc used to reference the NPC in LUA
   * @details In case of encounter, this will represent the index of the NPC in the spawning point NPC list
   * In case of neutral NPC, this represent the unique id of the NPC
   */
  std::uint32_t npc_lua_id;
};

struct script_engine::internal {

  internal() = default;

  sol::state lua;

  //! vector of encounter spawning points
  std::vector<spawning_point> spawning_points;

  //! vector of Spawned encounter, the index of the vector is the id of the corresponding spawning point
  std::vector<std::vector<npc_lua_instance>> spawned_per_spawning_point;

  std::vector<std::vector<npc_lua_instance>> neutral_npc_per_zone;
};

script_engine::script_engine()
    : _intern(std::make_unique<internal>()) {}

script_engine::~script_engine() = default;

npc_actions_report script_engine::execute_scripted_actions() {
  return {};
}

////////////////////////////////////////////////////////////////////////////////////
/// ================== World Populator Script Engine Builder =================== ///
////////////////////////////////////////////////////////////////////////////////////

void world_populator::generate_spawning_points(const std::string& spawning_point_config_path, const std::string& base_path) {
  std::ifstream i(spawning_point_config_path);
  auto jsonConfig = nlohmann::json::parse(i);
  auto wsJson = jsonConfig["spawning_points"];
  unsigned index = 0;

  _script_engine->_intern->spawning_points.resize(wsJson.size());
  for (auto& [key, value] : wsJson.items()) {
    const std::string keyCml = value["key_zone"].get<std::string>();
    const std::string sp_namespace = get_sp_namespace_from_key(keyCml);

    _script_engine->_intern->spawning_points[index].spawning_interval = std::chrono::seconds(value["spawning_interval"].get<uint>());
    _script_engine->_intern->spawning_points[index].id_spawning_point = sp_namespace;
    _script_engine->_intern->spawning_points[index].display_key = value["display_key"].get<std::string>();

    try {
      _script_engine->_intern->lua.safe_script_file(get_path_from_key(base_path, keyCml));
      auto maxSpawn = _script_engine->_intern->lua[sp_namespace]["numbers"];
      auto visibilityDistance = _script_engine->_intern->lua[sp_namespace]["visibility_distance"];
      auto centerPos_y = _script_engine->_intern->lua[sp_namespace]["center_point"]["y"];
      auto centerPos_x = _script_engine->_intern->lua[sp_namespace]["center_point"]["x"];

      if (!visibilityDistance.valid()) {
        throw std::runtime_error("'visibility_distance' is not properly set'");
      }
      if (!maxSpawn.valid()) {
        throw std::runtime_error("'maxSpawn' is not properly set");
      }
      if (!centerPos_x.valid(), !centerPos_y.valid()) {
        throw std::runtime_error("'center_pos' is not properly set");
      }
      _script_engine->_intern->spawning_points[index].center_spawning_point = pos{
          static_cast<double>(centerPos_x),
          static_cast<double>(centerPos_y)};
      _script_engine->_intern->spawning_points[index].max_spawned = static_cast<uint>(maxSpawn);
      _script_engine->_intern->spawning_points[index].distance_notification = static_cast<uint>(visibilityDistance);

    } catch (const std::exception& e) {
      log_error(fmt::format("[INIT] An error occurred while instantiating SpawningPoints '{}' : {}",
                            _script_engine->_intern->spawning_points[index].display_key, e.what()));
    }
    ++index;
  }
}

void world_populator::register_common_lua_engine(const std::string& to_lua_init_file) {
  _script_engine->_intern->lua.open_libraries(sol::lib::base, sol::lib::package);

  try {
    _script_engine->_intern->lua.safe_script_file(to_lua_init_file);
  } catch (const std::exception& e) {
    log_error(fmt::format("Error while initiating LUA engine : {} ", e.what()));
    throw;
  }

  auto position = _script_engine->_intern->lua.new_usertype<pos>("Pos");
  position["x"] = &pos::x;
  position["y"] = &pos::y;

  auto characterInfo = _script_engine->_intern->lua.new_usertype<character_info>("CharacterInfo");
  characterInfo["pos"] = &character_info::position;
  characterInfo["velocity"] = &character_info::velocity;
  characterInfo["angle"] = &character_info::angle;

  _script_engine->_intern->lua["retrieveAngle"] = [](double x, double y, double destinationX, double destinationY) {
    return std::atan((y - destinationY) / (x - destinationX));
  };
}

const std::vector<spawning_point>&
world_populator::get_spawning_point() const {
  return _script_engine->_intern->spawning_points;
}

script_engine& world_populator::get_script_engine() const {
  return *_script_engine;
}

}// namespace fys::ws