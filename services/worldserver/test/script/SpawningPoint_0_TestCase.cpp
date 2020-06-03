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

#include <fmt/format.h>
#include <catch2/catch.hpp>

#include <sol/state.hpp>

namespace {

[[nodiscard]] std::string
getScriptFilePath()
{
	std::string file_path = __FILE__;
	std::string dir_path = file_path.substr(0, file_path.rfind('\\'));
	if (dir_path.size() == file_path.size())
		dir_path = file_path.substr(0, file_path.rfind('/'));
	return dir_path + "/../../../../scripting_cache/scripts/ws/sp/SpawningPoint_0.lua";
}

[[nodiscard]] std::string
getInitFilePath()
{
	std::string file_path = __FILE__;
	std::string dir_path = file_path.substr(0, file_path.rfind('\\'));
	if (dir_path.size() == file_path.size())
		dir_path = file_path.substr(0, file_path.rfind('/'));
	return dir_path + "/initialization.lua";
}

}

TEST_CASE("SpawningPoint_0 TestCase", "[service][world][script]")
{
	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::package);

	try {
		lua.script_file(getInitFilePath());
		lua.script_file(getScriptFilePath());
	}
	catch (const std::exception& e) {
		FAIL(e.what());
	}

	SECTION("Create Encounter 0") {

		try {
			auto result0 = lua["spawn"](lua["spawningPoint_0"]);
			auto result1 = lua["spawn"](lua["spawningPoint_0"]);

			REQUIRE(result0.valid());
			REQUIRE(0 == static_cast<int>(result0));

			auto numbers = lua["spawningPoint_0"]["numbers"];
			REQUIRE(numbers.valid());
			REQUIRE(5 == static_cast<uint>(numbers));

			auto firstEncounter = lua["spawningPoint_0"]["spawned"][0];
			REQUIRE(firstEncounter.valid());
			REQUIRE(0 == static_cast<uint>(firstEncounter["idSpawningPoint"]));
			REQUIRE(4 == static_cast<uint>(firstEncounter["numberSteps"]));
			REQUIRE(10 == static_cast<uint>(firstEncounter["initial_info"]["x"]));
			REQUIRE(20 == static_cast<uint>(firstEncounter["initial_info"]["y"]));
			REQUIRE(1. == static_cast<double>(firstEncounter["initial_info"]["velocity"]));
			REQUIRE(0.5 == static_cast<double>(firstEncounter["initial_info"]["angle"]));

			REQUIRE(result1.valid());
			REQUIRE(1 == static_cast<int>(result1));

			lua.safe_script("print_test()");

		}
		catch (const std::exception& e) {
			FAIL(e.what());
		}
	} // End section : Create Encounter 0


} // End TestCase : SpawningPoint_0 TestCase