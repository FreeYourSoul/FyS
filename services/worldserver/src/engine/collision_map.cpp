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

#include "engine/collision_map.hh"
#include <iostream>
#include <spdlog/spdlog.h>
#include <tmxlite/Layer.hpp>
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <world_server_context.hh>

namespace fys::ws {

namespace {

[[nodiscard]] unsigned
getX(double x, unsigned tileSizeX) {
  static const unsigned gTileSizeX = tileSizeX;
  return static_cast<unsigned>(x) / gTileSizeX;
}

[[nodiscard]] unsigned
getY(double y, unsigned tileSizeY) {
  static const unsigned gTileSizeY = tileSizeY;
  return static_cast<unsigned>(y) / gTileSizeY;
}
}// namespace

// CollisionMap
collision_map::collision_map(const world_server_context& ctx)
	: _boundary_x(ctx.server_x_boundaries()),
	  _boundary_y(ctx.server_y_boundaries()),
	  _server_proximity(ctx.server_proximity()) {
  build_map_from_tmx(ctx.tmx_map_path());
}

void collision_map::build_map_from_tmx(const std::string& tmx_map_path) {
  tmx::Map map;
  if (!map.load(tmx_map_path)) {
	SPDLOG_ERROR("TMX CollisionMap couldn't be loaded");
	return;
  }
  const auto& layers = map.getLayers();

  _map_elems.resize(map.getTileCount().y);
  for (auto& elemOnY : _map_elems)
	elemOnY.resize(map.getTileCount().x);

  for (const auto& layer : layers) {
	if (layer->getType() == tmx::Layer::Type::Object) {
	  const auto& objectLayer = layer->getLayerAs<tmx::ObjectGroup>();

	  if (map::algo::is_collision_layer(objectLayer)) {
		add_collision_in_map({map.getTileSize().x, map.getTileSize().y}, objectLayer);
	  } else if (map::algo::is_trigger_layer(objectLayer)) {
		add_trigger_in_map(objectLayer);
	  }
	}
  }
}

void collision_map::add_collision_in_map(const vec2_u& tile_map_size, const tmx::ObjectGroup& collision_layer) {
  const auto& objects = collision_layer.getObjects();
  for (const auto& object : objects) {
	for (auto y = getX(object.getAABB().top, tile_map_size.y);
		 y < getX(object.getAABB().top + object.getAABB().height, tile_map_size.y); ++y) {
	  for (auto x = getY(object.getAABB().left, tile_map_size.x);
		   x < getY(object.getAABB().left + object.getAABB().width, tile_map_size.x); ++x) {
		_map_elems[y][x].set_type(e_element_type::BLOCK);
		_map_elems[y][x].add_collision(object.getAABB());
	  }
	}
  }
}

void collision_map::add_trigger_in_map(const tmx::ObjectGroup& trigger_layer) {
  // TODO : Add triggers on the map
}

void collision_map::execute_potential_trigger(std::uint32_t index, const character_info& position_on_map) {
}

bool collision_map::can_move_to(pos pos, std::size_t level) const noexcept {
  if (pos.x < _boundary_x.in || pos.x > _boundary_x.out || pos.y < _boundary_y.in || pos.y > _boundary_y.out)
	return false;
  return _map_elems[static_cast<unsigned long>(pos.x)][static_cast<unsigned long>(pos.y)].can_go_through(pos, level);
}

// CollisionMap Element
void map_element::execute_potential_trigger(std::uint32_t indexPlayer) const {
  if (_type == e_element_type::TRIGGER) {

  } else if (_type == e_element_type::TP_TRIGGER) {
  }
}

bool map_element::can_go_through(pos position, std::size_t level) const noexcept {
  bool canGoThrough = can_go_to_level(level);
  if (canGoThrough && _type == e_element_type::BLOCK) {
	return std::none_of(_collisions.begin(), _collisions.end(), [&position](const auto& aabb) {
	  return (position.x >= aabb.left && position.x <= aabb.left + aabb.width && position.y >= aabb.top && position.y <= aabb.top + aabb.height);
	});
  }
  return canGoThrough;
}

// todo : This isn't a good way to handle level, It has way to glitch if a player in the middle go to a stair.
//  We need to set some kind of isInTransition state in order to differentiate a character taking the stairs and
//  a character trying to pass from an intermediate level directly to the stairs.
bool map_element::can_go_to_level(std::size_t go_level) const noexcept {
  return _level.test(go_level) || _change_level.test(go_level);
}

}// namespace fys::ws
