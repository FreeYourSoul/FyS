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

#include <filesystem>
#include <fstream>

#include <catch2/catch.hpp>
#include <fmt/format.h>

#include <collision_map_converter.hh>
#include <engine/collision_map.hh>

namespace {
static const std::string test_collision_map_content = R"colmap(
{
    "map": [
        {
            "collisions": [],
            "full_collision": false,
            "triggers": []
        },
        {
            "collisions": [],
            "full_collision": true,
            "triggers": []
        },
        {
            "collisions": [
                [
                  64.0,   0.0,
                  64.0,  128.0
                ]
            ],
            "full_collision": false,
            "triggers": []
        },
        {
            "collisions": [],
            "full_collision": false,
            "triggers": []
        }
    ],
    "map_size": [ 2, 2 ],
    "tile_size": [ 128, 128 ]
}
)colmap";

std::string create_tmp_file(std::string name, const std::string& content) {
  auto tmp_file = std::filesystem::temp_directory_path() / std::move(name);
  std::error_code ec;
  std::filesystem::remove(tmp_file, ec);
  std::ofstream out(tmp_file);

  out << content;
  out.close();

  fmt::print("tmp file created for collision_map testing {}\n", tmp_file.string());
  return tmp_file.string();
}

}// namespace

TEST_CASE("collision_map TestCase", "[service][world][map]") {

  using namespace fys::ws;
  std::string file_path = create_tmp_file("collision_map_test_case.colmap", test_collision_map_content);
  fys::ws::collision_map cmap(file_path, boundary{0, 2}, boundary{0, 2});

  SECTION("test can_move_to success") {

    // Check all possibility in [0, 0]
    for (double i = 0.0; i < 1.0; i += 0.1) {
      for (double j = 0.0; j < 1.0; j += 0.1) {
        REQUIRE(cmap.can_move_to({i, j}));
      }
    }

    // Check all possibility in [1, 1]
    for (double i = 1.0; i < 2.0; i += 0.1) {
      for (double j = 1.0; j < 2.0; j += 0.1) {
        REQUIRE(cmap.can_move_to({i, j}));
      }
    }

  }// End section : test can_move_to success

  SECTION("test can_move_to failure: FULL_BLOCK") {

    // Check all possibility in [1, 0]
    for (double i = 1.0; i < 2.0; i += 0.1) {
      for (double j = 0.0; j < 1.0; j += 0.1) {
        REQUIRE_FALSE(cmap.can_move_to({i, j}));
      }
    }

  }

  SECTION("test can_move_to failure: BLOCK") {

    // Check in [0, 1] from [0.0 , 1.0] to [0.5 , 2.0] No Collision
    for (double i = 0.0; i < 0.5; i += 0.1) {
      for (double j = 1.0; j < 2.0; j += 0.1) {
        bool v = cmap.can_move_to({i, j});
        if (!v) {
          fmt::print("i {} j {}\n", i, j);
        }
        REQUIRE(v);
      }
    }

    // Check in [0, 1] from [0.5, 1.0] to [1.0, 2.0] Collision
    for (double i = 0.5; i < 1.0; i += 0.1) {
      for (double j = 1.0; j < 2.0; j += 0.1) {
        bool v = cmap.can_move_to({i, j});
        if (v) {
          fmt::print("i {} j {}\n", i, j);
        }
        REQUIRE_FALSE(v);
      }
    }
  }

}// End TestCase : collision_map TestCase

TEST_CASE("collision_map WorldMap Sized TestCase", "[service][world][map]") {

  using namespace fys::ws;
  std::string file_path = create_tmp_file("collision_map_test_case_size.colmap", test_collision_map_content);
  fys::ws::collision_map cmap(file_path, boundary{150, 152}, boundary{300, 302});

  SECTION("test can_move_to success") {

    // Check all possibility in [0, 0]
    for (double i = 150.0; i < 151.0; i += 0.1) {
      for (double j = 300.0; j < 301.0; j += 0.1) {
        REQUIRE(cmap.can_move_to({i, j}));
      }
    }

    // Check all possibility in [1, 1]
    for (double i = 151.0; i < 152.0; i += 0.1) {
      for (double j = 301.0; j < 302.0; j += 0.1) {
        REQUIRE(cmap.can_move_to({i, j}));
      }
    }

  }// End section : test can_move_to success

  SECTION("test can_move_to failure: FULL_BLOCK") {

    // Check all possibility in [1, 0]
    for (double i = 151.0; i < 152.0; i += 0.1) {
      for (double j = 300.0; j < 301.0; j += 0.1) {
        REQUIRE_FALSE(cmap.can_move_to({i, j}));
      }
    }

  }

  SECTION("test can_move_to failure: BLOCK") {

    // Check in [0, 1] from [0.0 , 1.0] to [0.5 , 2.0] No Collision
    for (double i = 150.0; i < 150.5; i += 0.1) {
      for (double j = 301.0; j < 302.0; j += 0.1) {
        bool v = cmap.can_move_to({i, j});
        if (!v) {
          fmt::print("i {} j {}\n", i, j);
        }
        REQUIRE(v);
      }
    }

    // Check in [0, 1] from [0.5, 1.0] to [1.0, 2.0] Collision
    for (double i = 150.5; i < 151.0; i += 0.1) {
      for (double j = 301.0; j < 302.0; j += 0.1) {
        bool v = cmap.can_move_to({i, j});
        if (v) {
          fmt::print("i {} j {}\n", i, j);
        }
        REQUIRE_FALSE(v);
      }
    }
  }

}// End TestCase : collision_map TestCase