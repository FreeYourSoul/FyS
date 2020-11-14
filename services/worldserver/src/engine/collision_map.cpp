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

struct collision_map::internal {

  explicit internal(const world_server_context& ctx)
      : _boundary_x(ctx.server_x_boundaries()),
        _boundary_y(ctx.server_y_boundaries()),
        _server_proximity(ctx.server_proximity()) {}

//  [[nodiscard]] unsigned
//  get_x(double x) {
//    static const unsigned gTileSizeX = _;
//    return static_cast<unsigned>(x) / gTileSizeX;
//  }
//
//  [[nodiscard]] unsigned
//  get_y(double y) {
//    static const unsigned gTileSizeY = tileSizeY;
//    return static_cast<unsigned>(y) / gTileSizeY;
//  }

  map_element get(unsigned long x, unsigned long y) {
    return {};
  }

/**
 * @brief Use a provided file into the collision map format (see collision_map_converter) and fill the collision map
 *
 * @param map_file_path path to a collision map format
 */
  void build_map_from_file(const std::string& map_file_path) {
    map_converter::transition_map transit = map_converter::retrieve_transition_map(map_file_path);

    for (unsigned i = 0; i < transit.map.size(); ++i) {
      const auto& elem = transit.map.at(i);

      if (elem.full_collision) {

      }
      else if (!elem.hb_collision.empty()) {

//        map_builder.add_collision(is_full, std::move(elem));
      }

      if (!elem.hb_trigger.empty()) {
//        map_builder.add_trigger(std::move(elem));
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
void map_element::execute_potential_trigger(std::uint32_t indexPlayer) const {
  if (_type == e_element_type::TRIGGER) {

  } else if (_type == e_element_type::TP_TRIGGER) {
  }
}

bool map_element::can_go_through(pos position) const noexcept {
  if (_type == e_element_type::FULL_BLOCK) {
    return false;
  }
  if (_type == e_element_type::BLOCK) {
    return std::none_of(_collisions.begin(), _collisions.end(), [&position](const auto& aabb) {
      return (position.x >= aabb.left
              && position.x <= aabb.left + aabb.width
              && position.y >= aabb.top
              && position.y <= aabb.top + aabb.height);
    });
  }
  return true;
}

// CollisionMap
collision_map::collision_map(const world_server_context& ctx)
    : _intern(std::make_unique<internal>(ctx)) {
  _intern->build_map_from_file(ctx.collision_map_path());
}

collision_map::~collision_map() = default;
collision_map::collision_map(collision_map&&) noexcept = default;

void collision_map::execute_potential_trigger(const pos& position, std::uint32_t index,
                                              const character_info& character) {
  if (_intern->get(position.x, position.y).type() == e_element_type::TP_TRIGGER) {
  }
  if (_intern->get(position.x, position.y).type() == e_element_type::TRIGGER) {
  }
}

bool collision_map::can_move_to(pos pos, std::size_t level) const noexcept {
  if (pos.x < _intern->_boundary_x.in || pos.x > _intern->_boundary_x.out
      || pos.y < _intern->_boundary_y.in || pos.y > _intern->_boundary_y.out) {
    return false;
  }
  return _intern->get(static_cast<unsigned long>(pos.x), static_cast<unsigned long>(pos.y)).can_go_through(pos);
}

}// namespace fys::ws
