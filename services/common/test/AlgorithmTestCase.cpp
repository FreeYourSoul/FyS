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

#include <catch2/catch.hpp>
#include <tuple>
#include <algorithm/algorithm.hh>

TEST_CASE("Test_find_more_suitable", "[common][algorithm]")
{

	SECTION("Empty") {
		std::vector<int> basic = {};
		auto min = [](int curr, int next) { return curr > next; };

		REQUIRE(basic.end() == fys::find_most_suitable(basic.begin(), basic.end(), min));
	}

	SECTION("Basic test") {
		std::vector<int> basic = {1, 3, 4, 5, 2, 1, 5, 6, 1, 33, 22, 1111, 34, 5};

		auto min = [](int curr, int next) { return curr > next; };
		auto max = [](int curr, int next) { return curr < next; };
		REQUIRE(basic.end() != fys::find_most_suitable(basic.begin(), basic.end(), max));
		REQUIRE(11 == std::distance(basic.begin(), fys::find_most_suitable(basic.begin(), basic.end(), max)));
		REQUIRE(1111 == *fys::find_most_suitable(basic.begin(), basic.end(), max));
		REQUIRE(basic.end() != fys::find_most_suitable(basic.begin(), basic.end(), min));
		REQUIRE(0 == std::distance(basic.begin(), fys::find_most_suitable(basic.begin(), basic.end(), min)));
		REQUIRE(1 == *fys::find_most_suitable(basic.begin(), basic.end(), min));
	}

	SECTION("Structure") {

		struct TestingStruct {
			int a;
			std::string b;
			double c;

			bool
			operator==(const TestingStruct& other) const
			{
				return std::make_tuple(a, b, c)
						== std::make_tuple(other.a, other.b, other.c);
			}
		};

		std::vector<TestingStruct> structVec =
				{
						{1, "salutzkk", 1.1},
						{1, "salutf3f3f3f3f", 1.1},
						{1, "salutf32", 1.1},
						{1, "salut3", 1.1}
				};

		TestingStruct longest{1, "salutf3f3f3f3f", 1.1};
		TestingStruct smallest{1, "salut3", 1.1};

		auto findLong = [](auto& curr, auto& next) { return curr.b.size() < next.b.size(); };
		auto findShort = [](auto& curr, auto& next) { return curr.b.size() > next.b.size(); };

		REQUIRE(structVec.end() != fys::find_most_suitable(structVec.begin(), structVec.end(), findLong));
		REQUIRE(1 == std::distance(structVec.begin(), fys::find_most_suitable(structVec.begin(), structVec.end(), findLong)));
		REQUIRE(longest == *fys::find_most_suitable(structVec.begin(), structVec.end(), findLong));
		REQUIRE(structVec.end() != fys::find_most_suitable(structVec.begin(), structVec.end(), findShort));
		REQUIRE(3 == std::distance(structVec.begin(), fys::find_most_suitable(structVec.begin(), structVec.end(), findShort)));
		REQUIRE(smallest == *fys::find_most_suitable(structVec.begin(), structVec.end(), findShort));
	}

}