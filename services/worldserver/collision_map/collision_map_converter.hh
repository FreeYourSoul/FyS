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
  bool full_collision = false;
};

struct transition_map {
  ws::vec2_u map_size;
  ws::vec2_u tile_size;

  ws::hitbox_d max_size_elem;
  std::vector<map_element> map;
};

void convert_map_from_tmx_file(const std::string& tmx_path, const std::string& destination, bool human_readable = false);

[[nodiscard]] transition_map retrieve_transition_map(const std::string& collision_map_path);

}// namespace fys::map_converter
#endif//FYS_ONLINE_SERVICES_WORLDSERVER_COLLISION_MAP_CONVERTER_COLLISION_MAP_CONVERTER_HH
