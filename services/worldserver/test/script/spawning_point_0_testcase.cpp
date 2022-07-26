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

#include <filesystem>

#include <catch2/catch.hpp>
#include <fmt/format.h>

#include <sol/state.hpp>

namespace {

[[nodiscard]] std::string getScriptFilePath() {
  return std::filesystem::current_path().string() + "/../../../../scripting_cache/scripts/ws/sp/spawningPoint_0.lua";
}

[[nodiscard]] std::string getInitFilePath() {
  return std::filesystem::current_path().string() + "/../../../../scripting_cache/scripts/ws/initialization.lua";
}

}// namespace

#include <chrono>
#include <thread>

TEST_CASE("SpawningPoint_0 TestCase", "[service][world][script]") {

  sol::state lua;
  lua.open_libraries(sol::lib::base, sol::lib::package);

  try {
    lua.script_file(getInitFilePath());
    lua.script_file(getScriptFilePath());
  } catch (const std::exception& e) {
    FAIL(e.what());
  }

  SECTION("Create Encounter 0") {

    try {
      auto result_0 = lua["spawn"](lua["spawningPoint_0"]);

      REQUIRE(result_0.valid());
      REQUIRE(0 == static_cast<int>(result_0));

      auto id_sp = lua["spawningPoint_0"]["idSpawningPoint"];
      REQUIRE(0 == static_cast<uint>(id_sp));

      auto numbers = lua["spawningPoint_0"]["numbers"];
      REQUIRE(numbers.valid());
      REQUIRE(1 == static_cast<uint>(numbers));

      auto first_encounter = lua["spawningPoint_0"]["spawned"][0];
      REQUIRE(4 == static_cast<uint>(first_encounter["numberSteps"]));
      REQUIRE(10 == static_cast<uint>(first_encounter["initial_info"]["x"]));
      REQUIRE(20 == static_cast<uint>(first_encounter["initial_info"]["y"]));
      REQUIRE(1. == static_cast<double>(first_encounter["initial_info"]["velocity"]));
      REQUIRE(0.5 == static_cast<double>(first_encounter["initial_info"]["angle"]));

    } catch (const std::exception& e) {
      FAIL(e.what());
    }
  }// End section : Create Encounter 0

}// End TestCase : SpawningPoint_0 TestCase