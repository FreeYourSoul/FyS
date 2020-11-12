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

#ifndef FYS_ONLINE_SERVICES_WORLDSERVER_COLLISION_MAP_CONVERTER_COLLISION_MAP_CONVERTER_HH
#define FYS_ONLINE_SERVICES_WORLDSERVER_COLLISION_MAP_CONVERTER_COLLISION_MAP_CONVERTER_HH

#include <optional>
#include <string>

#include "../include/engine/player_data.hh"

namespace fys::map_converter {

using trigger_box = std::pair<ws::hitbox_d, unsigned>;

struct map_element {
  std::vector<trigger_box> hb_trigger;
  std::vector<ws::hitbox_d> hb_collision;
};

struct transition_map {
  unsigned x = 0, y = 0;

  ws::hitbox_d max_size_elem;
  std::vector<map_element> map;
};

void convert_map_from_tmx_file(const std::string& tmx_path);
[[nodiscard]] transition_map retrieve_transition_map(const std::string& collision_map_path);

template<typename MapBuilder>
void build_from_collision_map(const std::string& collision_map_path, MapBuilder& map_builder) {
  transition_map transit = retrieve_transition_map(collision_map_path);

  map_builder.init_size(transit.x, transit.y);

  for (unsigned i = 0; i < transit.map.size(); ++i) {
	const auto& elem = transit.map.at(i);

	if (!elem.hb_collision.empty()) {
	  bool is_full = elem.hb_collision.front().height == transit.max_size_elem.height
				  && elem.hb_collision.front().width == transit.max_size_elem.width;

	  map_builder.add_collision(is_full, std::move(elem));
	}
	if (!elem.hb_trigger.empty()) {
	  map_builder.add_trigger(std::move(elem));
	}
  }
}

}// namespace fys::map_converter
#endif//FYS_ONLINE_SERVICES_WORLDSERVER_COLLISION_MAP_CONVERTER_COLLISION_MAP_CONVERTER_HH
