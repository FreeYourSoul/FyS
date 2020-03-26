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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-msc32-c"

#include <catch2/catch.hpp>
#include <ArenaServerContext.hh>
#include "TestType.hh"

using namespace fys::arena;

// Test of the game loop

TEST_CASE("FightingPit test", "[service][arena]")
{
	auto fseamMock = FSeam::getDefault<fys::util::RandomGenerator>();
	ConnectionHandler handler{};
	auto fseamConnectionHandlerMock = FSeam::get(&handler);
	auto cml = CmlBase(getLocalPathStorage());
	EncounterContext ctx;
	ctx._rangeEncounterPerZone["WS00"] = {
			std::make_pair(0, 0), // ez
			std::make_pair(0, 0), // medium
			std::make_pair(0, 0)  // hard
	};
	ctx._contendersPerZone["WS00"] = {
			EncounterContext::EncounterDesc{
					"arena:contenders:Sampy.chai", 3,
					{60, 60, 60}
			},
			EncounterContext::EncounterDesc{
					"arena:contenders:Slime.chai", 3,
					{40, 40, 40}
			}
	};

	// seed used 42
	std::shared_ptr<std::mt19937> mt = std::make_shared<std::mt19937>(42);
	fseamMock->dupeReturn<FSeam::RandomGenerator::get>(mt);

	REQUIRE(0 == fys::util::RandomGenerator::generateInRange(0, 0));

} // End TestCase : FightingPit test

#pragma clang diagnostic pop