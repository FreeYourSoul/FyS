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

#include <bitset>
#include <optional>
#include <variant>
#include <vector>

#include "player_data.hh"

// forward declaration
namespace fys::ws {
class world_server_context;
class connection_handler;
}// namespace fys::ws
// end forward declaration

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

  void add_collision(const hitbox_d& object) {
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
  std::vector<hitbox_d> _collisions;
};

class collision_map {
  struct internal;

public:
  explicit collision_map(const world_server_context& ctx);
  ~collision_map();

  collision_map(collision_map&&) noexcept;
  collision_map(const collision_map&) = delete;
  collision_map& operator=(const collision_map&) = delete;

  void build_map_from_file(const std::string& map_file_path);
  void execute_potential_trigger(const pos& position, std::uint32_t index, const character_info& position_on_map);

  /**
   * Check if the position is in the boundary of the map before checking on the map
   * @return true if it is possible to move on the given position, false otherwise
   */
  [[nodiscard]] bool
  can_move_to(pos pos, std::size_t level) const noexcept;

private:
  std::unique_ptr<internal> _intern;
};

}// namespace fys::ws

#endif//FYS_COLLISIONMAP_HH
