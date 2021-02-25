// MIT License
//
// Copyright (c) 2021 Quentin Balland
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
using namespace fys::ws;

TEST_CASE("collision_map_test_case TestCase", "[service][world][map]") {
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

TEST_CASE("collision_map_test_case collision", "[service][world][map]") {

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

}// End TestCase : collision_map_test_case TestCase

TEST_CASE("collision_map_test_case proximity_server simple") {
  using psa = proximity_server::proximity_server_axis;

  std::string file_path = create_tmp_file("collision_map_test_case.colmap", test_collision_map_content);
  fys::ws::collision_map cmap(
      file_path, boundary{0, 2}, boundary{0, 2},
      {
          {"WS_NORTH", std::nullopt, psa{0.5, false}},
          {"WS_SOUTH", std::nullopt, psa{1.5, true}},
          {"WS_WEST", psa{0.5, false}, std::nullopt},
          {"WS_EAST", psa{1.5, true}, std::nullopt},
      });

  SECTION("no proximity") {
    auto prox = cmap.server_at_proximity(pos{1.0, 1.0});
    REQUIRE(prox.empty());
  }// End section : no proximity

  SECTION("north proximity") {
    auto prox = cmap.server_at_proximity(pos{1.0, 0.2});

    REQUIRE(1 == prox.size());
    REQUIRE("WS_NORTH" == prox.at(0).code);
  }// End section : north proximity

  SECTION("south proximity") {
    auto prox = cmap.server_at_proximity(pos{1.0, 1.7});

    REQUIRE(1 == prox.size());
    REQUIRE("WS_SOUTH" == prox.at(0).code);
  }// End section : south proximity

  SECTION("east proximity") {
    auto prox = cmap.server_at_proximity(pos{1.7, 1.0});

    REQUIRE(1 == prox.size());
    REQUIRE("WS_EAST" == prox.at(0).code);
  }// End section : east proximity

  SECTION("west proximity") {
    auto prox = cmap.server_at_proximity(pos{0.2, 1.0});

    REQUIRE(1 == prox.size());
    REQUIRE("WS_WEST" == prox.at(0).code);
  }// End section : west proximity

  SECTION("south_east proximity") {
    auto prox = cmap.server_at_proximity(pos{1.7, 1.7});

    REQUIRE(2 == prox.size());
    REQUIRE("WS_SOUTH" == prox.at(0).code);
    REQUIRE("WS_EAST" == prox.at(1).code);
  }// End section : south_east proximity

  SECTION("nort_east proximity") {
    auto prox = cmap.server_at_proximity(pos{1.7, 0.2});

    REQUIRE(2 == prox.size());
    REQUIRE("WS_NORTH" == prox.at(0).code);
    REQUIRE("WS_EAST" == prox.at(1).code);
  }// End section : north_east proximity

  SECTION("south_west proximity") {
    auto prox = cmap.server_at_proximity(pos{0.2, 1.7});

    REQUIRE(2 == prox.size());
    REQUIRE("WS_SOUTH" == prox.at(0).code);
    REQUIRE("WS_WEST" == prox.at(1).code);
  }// End section : west_east proximity

  SECTION("nort_west proximity") {
    auto prox = cmap.server_at_proximity(pos{0.2, 0.2});

    REQUIRE(2 == prox.size());
    REQUIRE("WS_NORTH" == prox.at(0).code);
    REQUIRE("WS_WEST" == prox.at(1).code);
  }// End section : west_east proximity

}// End TestCase : collision_map_test_case proximity_server

TEST_CASE("collision_map_test_case proximity_server corner") {
  using psa = proximity_server::proximity_server_axis;

  std::string file_path = create_tmp_file("collision_map_test_case_corner.colmap", test_collision_map_content);
  fys::ws::collision_map cmap(
      file_path, boundary{0, 2}, boundary{0, 2},
      {
          {"WS_NORTH", std::nullopt, psa{0.5, false}},
          {"WS_SOUTH", std::nullopt, psa{1.5, true}},
          {"WS_WEST", psa{0.5, false}, std::nullopt},
          {"WS_EAST", psa{1.5, true}, std::nullopt},

          {"WS_NORTH_WEST", psa{0.5, false}, psa{0.5, false}},
          {"WS_SOUTH_WEST", psa{0.5, false}, psa{1.5, true}},
          {"WS_NORTH_EAST", psa{1.5, true}, psa{0.5, false}},
          {"WS_SOUTH_EAST", psa{1.5, true}, psa{1.5, true}},
      });

  SECTION("no proximity") {
    auto prox = cmap.server_at_proximity(pos{1.0, 1.0});
    REQUIRE(prox.empty());
  }// End section : no proximity

  SECTION("north proximity") {
    auto prox = cmap.server_at_proximity(pos{1.0, 0.2});

    REQUIRE(1 == prox.size());
    REQUIRE("WS_NORTH" == prox.at(0).code);
  }// End section : north proximity

  SECTION("south proximity") {
    auto prox = cmap.server_at_proximity(pos{1.0, 1.7});

    REQUIRE(1 == prox.size());
    REQUIRE("WS_SOUTH" == prox.at(0).code);
  }// End section : south proximity

  SECTION("east proximity") {
    auto prox = cmap.server_at_proximity(pos{1.7, 1.0});

    REQUIRE(1 == prox.size());
    REQUIRE("WS_EAST" == prox.at(0).code);
  }// End section : east proximity

  SECTION("west proximity") {
    auto prox = cmap.server_at_proximity(pos{0.2, 1.0});

    REQUIRE(1 == prox.size());
    REQUIRE("WS_WEST" == prox.at(0).code);
  }// End section : west proximity

  SECTION("south_east proximity") {
    auto prox = cmap.server_at_proximity(pos{1.7, 1.7});

    REQUIRE(3 == prox.size());
    REQUIRE("WS_SOUTH" == prox.at(0).code);
    REQUIRE("WS_EAST" == prox.at(1).code);
    REQUIRE("WS_SOUTH_EAST" == prox.at(2).code);
  }// End section : south_east proximity

  SECTION("nort_east proximity") {
    auto prox = cmap.server_at_proximity(pos{1.7, 0.2});

    REQUIRE(3 == prox.size());
    REQUIRE("WS_NORTH" == prox.at(0).code);
    REQUIRE("WS_EAST" == prox.at(1).code);
    REQUIRE("WS_NORTH_EAST" == prox.at(2).code);
  }// End section : north_east proximity

  SECTION("south_west proximity") {
    auto prox = cmap.server_at_proximity(pos{0.2, 1.7});

    REQUIRE(3 == prox.size());
    REQUIRE("WS_SOUTH" == prox.at(0).code);
    REQUIRE("WS_WEST" == prox.at(1).code);
    REQUIRE("WS_SOUTH_WEST" == prox.at(2).code);
  }// End section : west_east proximity

  SECTION("nort_west proximity") {
    auto prox = cmap.server_at_proximity(pos{0.2, 0.2});

    REQUIRE(3 == prox.size());
    REQUIRE("WS_NORTH" == prox.at(0).code);
    REQUIRE("WS_WEST" == prox.at(1).code);
    REQUIRE("WS_NORTH_WEST" == prox.at(2).code);
  }// End section : west_east proximity

}// End TestCase : collision_map_test_case proximity_server corner