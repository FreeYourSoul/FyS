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

enum class e_element_type : int {
  BLOCK = 0,
  FULL_BLOCK = 1,
  TRIGGER = 2,
  MAP_TRIGGER = 3,
};

class map_element {

public:
  /**
   * check if the element is of type BLOCK, if it is, check every collision blockers on the mapElement
   * to verify if the PlayerInfo collide with them.
   *
   * @param relative_position position relative to the current map (opposite of worldmap position)
   * @param tile_size size of a tile for this map (to use percentage on the position)
   * @return True if it is possible to go into this position for the element, false otherwise
   */
  [[nodiscard]] inline bool
  can_go_through(pos relative_position, const vec2_d &tile_size) const noexcept;

  void add_collision(const hitbox_d& object) {
    _collisions.emplace_back(object);
  }

  template<typename T>
  void add_trigger(T&& trigger, hitbox_d box) {
//    _triggers.emplace_back(box, std::forward(trigger));
  }

  void set_type(e_element_type type) { _type.set(int(type), true); }

  bool is_empty() const { return _type.none(); }
  bool is(e_element_type to_check) const { return _type[int(to_check)]; }
  bool isor(const std::vector<e_element_type>& to_check) const {
    return std::any_of(to_check.begin(), to_check.end(), [this](e_element_type t) { return _type[int(t)]; });
  }

private:
  std::bitset<4> _type;
  std::vector<std::pair<hitbox_d, std::variant<connection_handler*, void*>>> _triggers{};
  std::vector<hitbox_d> _collisions{};
};

class collision_map {
  struct internal;

public:
  explicit collision_map(const world_server_context& ctx);
  collision_map(const std::string& map, boundary x, boundary y, std::vector<proximity_server> prox = {});
  ~collision_map();

  collision_map(collision_map&&) noexcept;
  collision_map(const collision_map&) = delete;
  collision_map& operator=(const collision_map&) = delete;

  /**
   * @brief Execute all triggers that should be executed on the given movement for a given character
   * @param worldmap_pos worldmap position in which the given character move
   * @param index unique index of the character
   * @param character information of the character moving
   */
  void execute_potential_trigger(const pos& worldmap_pos, std::uint32_t index, const character_info& character);

  /**
   * @brief Check if the given worldmap position is in the boundary of the map then check if it is possible to
   * move to this worldmap position.
   *
   * @param world_map_pos worldmap position to check if it possible to move to
   * @return true if it is possible to move on the given position, false otherwise
   */
  [[nodiscard]] bool can_move_to(pos world_map_pos) const noexcept;

private:
  std::unique_ptr<internal> _intern;
};

}// namespace fys::ws

#endif//FYS_COLLISIONMAP_HH
