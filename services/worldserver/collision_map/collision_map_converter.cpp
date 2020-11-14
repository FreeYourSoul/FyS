// MIT License
//
// Copyright (c) 2020 Quentin Balland
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

#include <spdlog/spdlog.h>

#include <algorithm>
#include <exception>
#include <fstream>
#include <iomanip>

#include <fmt/format.h>

#include <nlohmann/json.hpp>

#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>

#include "collision_map_converter.hh"

namespace fys::map_converter {

namespace {
const tmx::Tileset& find_tileset_by_tile_id(const std::vector<tmx::Tileset>& tilesets, std::uint32_t id_tile) {

  auto it = std::find_if(tilesets.cbegin(), tilesets.cend(), [&id_tile](const auto& t) { return t.hasTile(id_tile); });
  if (it == tilesets.end()) {
	throw std::runtime_error(fmt::format("id_tile {} cannot be found in tilesets", id_tile));
  }
  return *it;
}

unsigned get_id_property(const std::vector<tmx::Property>& properties) {
  auto it = std::find_if(properties.begin(), properties.end(), [](const auto& prop) { return prop.getName() == "id"; });
  if (it == properties.end()) {
	return 0;
  }
  return it->getIntValue();
}

void fill_map_element(map_element& element, const tmx::Tileset::Tile* tile, unsigned map_x, unsigned map_y) {
  const auto& objects = tile->objectGroup.getObjects();
  for (const auto tile_object : objects) {

	// Fill collision vector of map element
	if (!element.full_collision
		&& (tile_object.getType().empty()
			|| tile_object.getType() == "collision")) {

	  const auto& rec = tile_object.getAABB();
	  if (rec.top == 0 && rec.height == 0
		  && static_cast<unsigned>(rec.width) == map_x
		  && static_cast<unsigned>(rec.height) == map_y) {
		element.full_collision = true;
	  } else {
		fmt::print("> Collision found {}\n", tile->ID);
		element.hb_collision.emplace_back(rec.left, rec.top, rec.width, rec.height);
	  }
	}

	  // Fill trigger vector of map element
	else if (tile_object.getType() == "trigger") {
	  const auto& rec = tile_object.getAABB();
	  const unsigned id_trigger = get_id_property(tile_object.getProperties());
	  if (id_trigger == 0) {
		throw std::runtime_error(
			fmt::format("Tile {} has a Trigger type object that doesn't have an id property", tile->ID));
	  }
	  element.hb_trigger.emplace_back(ws::hitbox_d(rec.left, rec.top, rec.width, rec.height), id_trigger);
	}

  }
}

void serialize_in_file(transition_map transition_map, const std::string& destination_path) {

  nlohmann::json serializer;

  serializer["map_boundary"] = std::pair(transition_map.x, transition_map.y);
  serializer["map"] = nlohmann::json::array();

  auto collision_to_json = [](const std::vector<ws::hitbox_d>& collisions) {
	nlohmann::json json_elem;
	json_elem = nlohmann::json::array();
	for (const auto& collision : collisions) {
	  json_elem.emplace_back(std::tuple(collision.left, collision.top, collision.width, collision.height));
	}
	return json_elem;
  };

  auto triggers_to_json = [](const std::vector<trigger_box>& triggers) {
	nlohmann::json json_elem;
	json_elem = nlohmann::json::array();
	for (const auto& trigger : triggers) {
	  nlohmann::json json_t;
	  json_t["id"] = trigger.second;
	  json_t["zone"] = std::tuple(trigger.first.left, trigger.first.top, trigger.first.width, trigger.first.height);
	  json_elem.emplace_back(std::move(json_t));
	}
	return json_elem;
  };

  for (auto && elem : transition_map.map) {
	nlohmann::json json_elem;
	json_elem["full_collision"] = elem.full_collision;
	json_elem["collisions"] = collision_to_json(elem.hb_collision);
	json_elem["triggers"] = triggers_to_json(elem.hb_trigger);
	serializer["map"].emplace_back(std::move(json_elem));
  }

  std::ofstream o(destination_path);
  o << std::setw(4) << serializer << std::endl;

}

}// namespace

void convert_map_from_tmx_file(const std::string& tmx_path, const std::string& destination) {
  tmx::Map map;
  if (!map.load(tmx_path)) {
	SPDLOG_ERROR("TMX CollisionMap couldn't be loaded");
	return;
  }
  const auto& layers = map.getLayers();
  if (layers.empty()) {
	SPDLOG_ERROR("TMX Map should have at least one layer");
	return;
  }
  auto [x_bound, y_bound] = map.getTileCount();
  const unsigned max_size = x_bound * y_bound;
  std::vector<map_element> result;

  result.reserve(max_size);
  for (unsigned map_elem_index = 0; map_elem_index < max_size; ++map_elem_index) {
	map_element element{};

	for (const tmx::Layer::Ptr& layer : layers) {
	  if (layer->getType() != tmx::Layer::Type::Tile) {
		continue;
	  }

	  const auto& tile_layer = layer->getLayerAs<tmx::TileLayer>();

	  if (const std::uint32_t id_tile = tile_layer.getTiles().at(map_elem_index).ID; id_tile > 0) {
		const tmx::Tileset& tileset = find_tileset_by_tile_id(map.getTilesets(), id_tile);
		fill_map_element(element, tileset.getTile(id_tile), x_bound, y_bound);
	  }
	}

	// optimization on collision/trigger shapes can be done here

	result.emplace_back(std::move(element));
  }

  serialize_in_file(transition_map{x_bound, y_bound, ws::hitbox_d{}, std::move(result)}, destination);
}

}// namespace fys::map_converter