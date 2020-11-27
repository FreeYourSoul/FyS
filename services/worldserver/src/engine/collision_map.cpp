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

#include <algorithm>
#include <fstream>
#include <iostream>
#include <spdlog/spdlog.h>

#include <nlohmann/json.hpp>

#include <collision_map_converter.hh>
#include <world_server_context.hh>

#include "engine/collision_map.hh"

namespace fys::ws {

namespace {

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
  can_go_through(pos relative_position, const vec2_d& tile_size) const noexcept;

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
    return std::ranges::any_of(to_check, [this](e_element_type t) { return _type[int(t)]; });
  }

private:
  std::bitset<4> _type;
  std::vector<std::pair<hitbox_d, std::variant<connection_handler*, void*>>> _triggers{};
  std::vector<hitbox_d> _collisions{};
};

}// anonymous namespace

struct collision_map::internal {

  explicit internal(boundary x, boundary y, const std::vector<proximity_server>& prox)
      : _boundary_x(x),
        _boundary_y(y),
        _server_proximity(prox) {}

  [[nodiscard]] unsigned
  get_x(unsigned long x) {
    if (x > _boundary_x.out) {
      SPDLOG_ERROR("Trying to retrieve X {} (from_world_map {}) while boundary is [{}:{}]",
                   x, _boundary_x.in, _boundary_x.out);
      return 0;
    }
    return x;
  }

  [[nodiscard]] unsigned
  get_y(unsigned long y) {
    if (y > _boundary_y.out) {
      SPDLOG_ERROR("Trying to retrieve Y {} while boundary is [{}:{}]",
                   y, _boundary_y.in, _boundary_y.out);
      return 0;
    }
    return y * (_boundary_x.out - _boundary_x.in);
  }

  void add_collisions(std::uint32_t index, const map_converter::map_element& element) {
    if (element.full_collision) {
      _map_elems[index].set_type(e_element_type::FULL_BLOCK);
      return;
    }
    _map_elems[index].set_type(e_element_type::BLOCK);
    for (const auto& hitbox : element.hb_collision) {
      _map_elems[index].add_collision(hitbox);
    }
  }

  void add_triggers(std::uint32_t index, const map_converter::map_element& element) {
    _map_elems[index].set_type(e_element_type::TRIGGER);
    for (const auto& [hitbox, id_trigger] : element.hb_trigger) {
      _map_elems[index].add_trigger([]() {}, hitbox);
    }
  }

  const map_element& get(unsigned long relative_x, unsigned long relative_y) {
    return _map_elems[get_x(relative_x) + get_y(relative_y)];
  }

  /**
 * @brief Use a provided file into the collision map format (see collision_map_converter) and fill the collision map
 *
 * @param map_file_path path to a collision map format
 */
  void build_map_from_file(const std::string& map_file_path) {
    map_converter::transition_map transit = map_converter::retrieve_transition_map(map_file_path);

    _tile_size.x = transit.tile_size.x;
    _tile_size.y = transit.tile_size.y;
    _map_elems.resize(transit.map.size());
    for (std::uint32_t i = 0; i < transit.map.size(); ++i) {
      const auto& elem = transit.map.at(i);

      if (elem.full_collision || !elem.hb_collision.empty()) {
        add_collisions(i, elem);
      }

      if (!elem.hb_trigger.empty()) {
        add_triggers(i, elem);
      }
    }
  }

  vec2_d _tile_size;
  boundary _boundary_x;
  boundary _boundary_y;
  std::vector<proximity_server> _server_proximity;
  std::vector<map_element> _map_elems;
};

// CollisionMap Element

bool map_element::can_go_through(pos relative_position, const vec2_d& tile_size) const noexcept {
  if (is(e_element_type::FULL_BLOCK)) {
    return false;
  }
  if (is(e_element_type::BLOCK)) {
    relative_position.x -= std::floor(relative_position.x);
    relative_position.y -= std::floor(relative_position.y);
    return std::ranges::none_of(_collisions, [&](const auto& aabb) {
      return (relative_position.x >= (aabb.left / tile_size.x)
              && relative_position.x <= ((aabb.left + aabb.width) / tile_size.x)
              && relative_position.y >= (aabb.top / tile_size.y)
              && relative_position.y <= ((aabb.top + aabb.height) / tile_size.y));
    });
  }
  return true;
}

// CollisionMap
collision_map::collision_map(const world_server_context& ctx)
    : _intern(std::make_unique<internal>(ctx.server_x_boundaries(),
                                         ctx.server_y_boundaries(),
                                         ctx.server_proximity())) {

  _intern->build_map_from_file(ctx.collision_map_path());
}

collision_map::collision_map(const std::string& map, boundary x, boundary y, std::vector<proximity_server> prox)
    : _intern(std::make_unique<internal>(x, y, prox)) {
  _intern->build_map_from_file(map);
}

collision_map::~collision_map() = default;
collision_map::collision_map(collision_map&&) noexcept = default;

void collision_map::execute_potential_trigger(const pos& worldmap_pos, std::uint32_t index,
                                              const character_info& character) {
  const auto& map_elem = _intern->get(worldmap_pos.x, worldmap_pos.y);
  if (map_elem.is(e_element_type::TRIGGER)) {

  } else if (map_elem.is(e_element_type::MAP_TRIGGER)) {
  }
}

bool collision_map::can_move_to(pos world_map_pos) const noexcept {
  if (world_map_pos.x < _intern->_boundary_x.in || world_map_pos.x > _intern->_boundary_x.out
      || world_map_pos.y < _intern->_boundary_y.in || world_map_pos.y > _intern->_boundary_y.out) {
    return false;
  }
  pos relative_pos = world_map_pos;
  relative_pos.x -= _intern->_boundary_x.in;
  relative_pos.y -= _intern->_boundary_y.in;

  const auto& elem = _intern->get(static_cast<unsigned long>(relative_pos.x),
                                  static_cast<unsigned long>(relative_pos.y));

  return elem.can_go_through(relative_pos, _intern->_tile_size);
}

std::vector<proximity_server> collision_map::server_at_proximity(pos pos) const noexcept {
  auto prox = _intern->_server_proximity
            | std::views::filter([&pos](const auto& sp) { return sp.is_close_by(pos); });

  std::vector<std::ranges::range_value_t<decltype(prox)>> v;
  std::ranges::copy(prox, std::back_inserter(v));
  return v;
}

}// namespace fys::ws
