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

#include <collision_map_converter.hh>

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
                    64.0,
                    0.0,
                    64.0,
                    128.0
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
    "map_size": [
        2,
        2
    ],
    "tile_size": [
        128,
        128
    ]
}
)colmap";

namespace {

std::string create_tmp_file(std::string name, const std::string& content) {
  auto tmp_file = std::filesystem::temp_directory_path() / std::move(name);
  std::error_code ec;
  std::filesystem::remove(tmp_file, ec);
  std::ofstream myfile;
  myfile.open(tmp_file);
  myfile << content;
  return tmp_file.string();
}

}

TEST_CASE("collision_map TestCase", "[service][world][map]") {

  std::string file_path = create_tmp_file("collision_map_test_case.colmap", test_collision_map_content);
  auto transition = fys::map_converter::retrieve_transition_map(file_path);

//  fys::ws::collision_map cmap();

}// End TestCase : collision_map TestCase