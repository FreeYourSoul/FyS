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
#include <string>
#include <BoundaryMap.hh>

TEST_CASE ("BoundaryMap test case", "[common][utils]") {

    fys::BoundaryMap<std::string> bmap;

    SECTION("test insert") {
        bmap.insert(1, "123456");
        bmap.insert(2, "321654");
        bmap.insert(3, "987654");

        REQUIRE("123456" == bmap.get(1)->second);
        REQUIRE("321654" == bmap.get(2)->second);
        REQUIRE("987654" == bmap.get(3)->second);
    }

    SECTION("test boundary get") {
        bmap.insert(10, "123456");
        bmap.insert(15, "321654");
        bmap.insert(42, "987654");

        REQUIRE("123456" == bmap.get(1)->second);
        REQUIRE(10 == bmap.get(1)->first);

        REQUIRE("123456" == bmap.get(2)->second);
        REQUIRE("123456" == bmap.get(3)->second);
        REQUIRE("123456" == bmap.get(4)->second);
        REQUIRE("123456" == bmap.get(5)->second);
        REQUIRE("123456" == bmap.get(6)->second);
        REQUIRE("123456" == bmap.get(7)->second);
        REQUIRE("123456" == bmap.get(8)->second);
        REQUIRE("123456" == bmap.get(9)->second);
        REQUIRE("123456" == bmap.get(10)->second);

        REQUIRE("321654" == bmap.get(11)->second);
        REQUIRE(15 == bmap.get(11)->first);

        REQUIRE("321654" == bmap.get(12)->second);
        REQUIRE("321654" == bmap.get(13)->second);
        REQUIRE("321654" == bmap.get(14)->second);
        REQUIRE("321654" == bmap.get(15)->second);

        REQUIRE("987654" == bmap.get(16)->second);
        REQUIRE(42 == bmap.get(16)->first);

        REQUIRE("987654" == bmap.get(17)->second);
        REQUIRE("987654" == bmap.get(18)->second);
        REQUIRE("987654" == bmap.get(19)->second);
        REQUIRE("987654" == bmap.get(20)->second);
        REQUIRE("987654" == bmap.get(21)->second);
        REQUIRE("987654" == bmap.get(22)->second);
        REQUIRE("987654" == bmap.get(23)->second);
        REQUIRE("987654" == bmap.get(24)->second);
        REQUIRE("987654" == bmap.get(25)->second);
        REQUIRE("987654" == bmap.get(26)->second);
        REQUIRE("987654" == bmap.get(27)->second);
        REQUIRE("987654" == bmap.get(28)->second);
        REQUIRE("987654" == bmap.get(29)->second);
        REQUIRE("987654" == bmap.get(30)->second);
        REQUIRE("987654" == bmap.get(31)->second);
        REQUIRE("987654" == bmap.get(32)->second);
        REQUIRE("987654" == bmap.get(33)->second);
        REQUIRE("987654" == bmap.get(34)->second);
        REQUIRE("987654" == bmap.get(35)->second);
        REQUIRE("987654" == bmap.get(36)->second);
        REQUIRE("987654" == bmap.get(37)->second);
        REQUIRE("987654" == bmap.get(38)->second);
        REQUIRE("987654" == bmap.get(39)->second);
        REQUIRE("987654" == bmap.get(40)->second);
        REQUIRE("987654" == bmap.get(41)->second);
        REQUIRE("987654" == bmap.get(42)->second);

        REQUIRE(bmap.end() == bmap.get(43));
        REQUIRE(bmap.end() == bmap.get(1337));
        REQUIRE(bmap.end() == bmap.get(1616));
        REQUIRE(bmap.end() == bmap.get(666));

    } // End Section : test boundary get

} // End Test Case : BoundaryMap test case
