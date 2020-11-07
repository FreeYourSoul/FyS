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

#ifndef FYS_COLLISIONMAP_HH
#define FYS_COLLISIONMAP_HH

#include "player_data.hh"
#include <algorithm>
#include <bitset>
#include <optional>
#include <tmxlite/ObjectGroup.hpp>
#include <tmxlite/Types.hpp>
#include <utility>
#include <variant>
#include <vector>

// forward declaration
namespace fys::ws {
class world_server_context;
class connection_handler;
}// namespace fys::ws
// end forward declaration

namespace fys::map::algo {

/**
 * Check if the given object layer is used for collision
 * @tparam TmxLayer tmx::GroupObjects layer
 * @param layer perfect forwarding object
 * @return true if the given group object layer is used for collision, false otherwise
 */
template<typename TmxLayer>
bool is_collision_layer(TmxLayer&& layer) {
  return std::any_of(layer.getProperties().begin(), layer.getProperties().end(), [](const auto& prop) {
	return prop.getName().find("collision") != std::string::npos;
  });
}

/**
 * Check if the given object layer is used for any trigger
 * @tparam TmxLayer tmx::GroupObjects layer
 * @param layer perfect forwarding object
 * @return true if the given group object layer is used for any trigger, false otherwise
 */
template<typename TmxLayer>
bool is_trigger_layer(TmxLayer&& layer) {
  return std::any_of(layer.getProperties().begin(), layer.getProperties().end(), [](const auto& prop) {
	return prop.getName().find("trigger") != std::string::npos;
  });
}

}// namespace fys::map::algo

namespace fys::ws {

struct boundary {
  double in = 0.0;
  double out = 0.0;
};

class proximity_server {
  struct proximity_server_axis {
	double value;
	bool superiorTo;
  };

public:
  [[nodiscard]] constexpr bool
  is_close_by(double axis) const noexcept {
	bool x_req = x_axis_requirement.has_value();
	bool y_req = y_axis_requirement.has_value();
	if (x_req)
	  x_req = (x_axis_requirement->superiorTo) ? (axis > x_axis_requirement->value) : (axis < x_axis_requirement->value);
	if (y_req)
	  y_req = (y_axis_requirement->superiorTo) ? (axis > y_axis_requirement->value) : (axis < y_axis_requirement->value);
	return x_req && y_req;
  }

  std::string code;
  std::optional<proximity_server_axis> x_axis_requirement = std::nullopt;
  std::optional<proximity_server_axis> y_axis_requirement = std::nullopt;
};

enum class e_element_type {
  BLOCK,
  TRIGGER,
  TP_TRIGGER,
  NONE
};

class map_element {

public:
  void execute_potential_trigger(std::uint32_t indexPlayer) const;

  void set_level(std::size_t level) { _level.set(level); }
  void set_change_level(std::size_t level) { _change_level.set(level); }

  void set_type(e_element_type type) { _type = type; }

  /**
   * check if the element is of type BLOCK, if it is, check every collision blockers on the mapElement
   * to verify if the PlayerInfo collide with them.
   */
  [[nodiscard]] inline bool
  can_go_through(pos position, std::size_t level) const noexcept;

  void add_collision(const tmx::FloatRect& object) {
	_collisions.emplace_back(object);
  }

  template<typename T>
  void set_trigger(T&& trigger) {
	_trigger = std::forward(trigger);
  }

private:
  [[nodiscard]] inline bool
  can_go_to_level(std::size_t go_level) const noexcept;

private:
  std::bitset<4> _level;
  std::bitset<4> _change_level;// set on stairs to pass from a level to another
  e_element_type _type = e_element_type::NONE;
  std::variant<connection_handler*, void*> _trigger;// TODO: change with std::function<void(LUA::Reference&)>
  std::vector<tmx::FloatRect> _collisions;
};

class collision_map {

public:
  explicit collision_map(const world_server_context& ctx);

  collision_map(collision_map&&) noexcept = default;
  collision_map(const collision_map&) = delete;
  collision_map& operator=(const collision_map&) = delete;

  void build_map_from_tmx(const std::string& tmx_map_path);
  void execute_potential_trigger(std::uint32_t index, const character_info& position_on_map);

  /**
   * Check if the position is in the boundary of the map before checking on the map
   * @return true if it is possible to move on the given position, false otherwise
   */
  [[nodiscard]] bool
  can_move_to(pos pos, std::size_t level) const noexcept;

private:
  /**
   * @brief Add the the collision elements (witht their AABB objects) into the map.
   *
   * AABB Objects stands for Axis-Aligned Bounding Box. Basically coordinates to use as hit box for the tiles.
   */
  void add_collision_in_map(const vec2_u& tile_map_size, const tmx::ObjectGroup& collision_layer);

  /**
   * @brief Add the trigger elements into the map, and link the function associated to this trigger
   *
   * A trigger is defined with an id, that is going to be checked against the database,
   *   - Some specific trigger (teleportation trigger) will trigger a complete C++ code that is going to
   *     teleport the player into another location
   *   - The classical one is going to trigger a script retrieved from the DB thanks to the id defining the trigger
   */
  void add_trigger_in_map(const tmx::ObjectGroup& trigger_layer);

private:
  boundary _boundary_x;
  boundary _boundary_y;
  std::vector<proximity_server> _server_proximity;
  std::vector<std::vector<map_element>> _map_elems;
};

}// namespace fys::ws

#endif//FYS_COLLISIONMAP_HH
