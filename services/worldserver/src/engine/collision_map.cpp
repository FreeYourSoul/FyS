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
#include <iostream>
#include <spdlog/spdlog.h>

#include <tmxlite/Layer.hpp>
#include <tmxlite/Map.hpp>
#include <tmxlite/ObjectGroup.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/Types.hpp>

#include <world_server_context.hh>

#include "engine/collision_map.hh"

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

[[nodiscard]] hitbox_d
to_hitobox(const tmx::FloatRect& rec) {
	return hitbox_d(rec.left, rec.top, rec.width, rec.height);
}

}// namespace

namespace map::algo {

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

}// namespace map::algo

struct collision_map::internal {

  explicit internal(const world_server_context& ctx)
	  : _boundary_x(ctx.server_x_boundaries()),
		_boundary_y(ctx.server_y_boundaries()),
		_server_proximity(ctx.server_proximity()) {}

  /**
   * @brief Add the collision elements (with their AABB objects) into the map.
   *
   * AABB Objects stands for Axis-Aligned Bounding Box. Basically coordinates to use as hit box for the tiles.
   */
  void add_collision_in_map(const vec2_u& tile_map_size, const tmx::ObjectGroup& collision_layer) {
	const auto& objects = collision_layer.getObjects();
	for (const auto& object : objects) {
	  for (auto y = getX(object.getAABB().top, tile_map_size.y);
		   y < getX(object.getAABB().top + object.getAABB().height, tile_map_size.y); ++y) {
		for (auto x = getY(object.getAABB().left, tile_map_size.x);
			 x < getY(object.getAABB().left + object.getAABB().width, tile_map_size.x); ++x) {
		  _map_elems[y][x].set_type(e_element_type::BLOCK);
		  _map_elems[y][x].add_collision(to_hitobox(object.getAABB()));
		}
	  }
	}
  }

  /**
   * @brief Add the trigger elements into the map, and link the function associated to this trigger
   *
   * A trigger is defined with an id, that is going to be checked against the database,
   *   - Some specific trigger (teleportation trigger) will trigger a complete C++ code that is going to
   *     teleport the player into another location
   *   - The classical one is going to trigger a script retrieved from the DB thanks to the id defining the trigger
   */
  void add_trigger_in_map(const tmx::ObjectGroup& trigger_layer) {
	// TODO : Add triggers on the map
  }

  boundary _boundary_x;
  boundary _boundary_y;
  std::vector<proximity_server> _server_proximity;
  std::vector<std::vector<map_element>> _map_elems;
};

// CollisionMap
collision_map::collision_map(const world_server_context& ctx)
	: _intern(std::make_unique<internal>(ctx)) {
  build_map_from_file(ctx.tmx_map_path());
}

collision_map::~collision_map() = default;
collision_map::collision_map(collision_map&&) noexcept = default;

void collision_map::build_map_from_file(const std::string& map_file_path) {
  tmx::Map map;
  if (!map.load(map_file_path)) {
	SPDLOG_ERROR("TMX CollisionMap couldn't be loaded");
	return;
  }
  const auto& layers = map.getLayers();

  _intern->_map_elems.resize(map.getTileCount().y);
  for (auto& elemOnY : _intern->_map_elems)
	elemOnY.resize(map.getTileCount().x);

  for (const auto& layer : layers) {
	if (layer->getType() == tmx::Layer::Type::Object) {
	  const auto& objectLayer = layer->getLayerAs<tmx::ObjectGroup>();

	  if (map::algo::is_collision_layer(objectLayer)) {
		_intern->add_collision_in_map({map.getTileSize().x, map.getTileSize().y}, objectLayer);
	  } else if (map::algo::is_trigger_layer(objectLayer)) {
		_intern->add_trigger_in_map(objectLayer);
	  }
	}
  }
}

void collision_map::execute_potential_trigger(const pos& position, std::uint32_t index, const character_info& position_on_map) {
}

bool collision_map::can_move_to(pos pos, std::size_t level) const noexcept {
  if (pos.x < _intern->_boundary_x.in || pos.x > _intern->_boundary_x.out
	  || pos.y < _intern->_boundary_y.in || pos.y > _intern->_boundary_y.out)
	return false;
  return _intern->_map_elems[static_cast<unsigned long>(pos.x)][static_cast<unsigned long>(pos.y)].can_go_through(pos, level);
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
