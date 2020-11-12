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

#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>

#include "collision_map_converter.hh"

namespace fys::map_converter {

void convert_map_from_tmx_file(const std::string& tmx_path) {
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
  std::vector<tmx::IntRect> res;

  for (unsigned x = 0; x < x_bound; ++x) {
	for (unsigned y = 0; y < y_bound; ++y) {

		layers.front()->getLayerAs<tmx::TileLayer>().getTiles();

	}

  }

}

}