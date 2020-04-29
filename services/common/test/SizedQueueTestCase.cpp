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
#include <SizedQueue.hh>

TEST_CASE("AlgorithmTestCase SizedQueue testing", "[common]")
{

	fys::common::SizedQueue<std::string, 7> strQueue;

	strQueue.push("1");
	strQueue.push("2");
	strQueue.push("3");
	strQueue.push("4");
	strQueue.push("5");

	SECTION("Test size") {
		REQUIRE(5 == strQueue.size());
		strQueue.push("6");
		REQUIRE(6 == strQueue.size());
		strQueue.pop();
		strQueue.pop();
		REQUIRE(4 == strQueue.size());

	} // End section : Test size

	SECTION("Push elem test") {
		REQUIRE(5 == strQueue.size());
		REQUIRE(strQueue.push("6"));
		REQUIRE(strQueue.push("7"));
		REQUIRE(7 == strQueue.size());
		REQUIRE_FALSE(strQueue.push("IMPOSSIBLE TO PUSH BECAUSE FULL QUEUE 1"));
		REQUIRE_FALSE(strQueue.push("IMPOSSIBLE TO PUSH BECAUSE FULL QUEUE 2"));
		REQUIRE_FALSE(strQueue.push("IMPOSSIBLE TO PUSH BECAUSE FULL QUEUE 3"));
		REQUIRE_FALSE(strQueue.push("IMPOSSIBLE TO PUSH BECAUSE FULL QUEUE 4"));
		REQUIRE_FALSE(strQueue.push("IMPOSSIBLE TO PUSH BECAUSE FULL QUEUE 5"));
		REQUIRE(7 == strQueue.size());

	} // End section : Push more elem test

	SECTION("Test pop") {
		REQUIRE(5 == strQueue.size());

		auto one = strQueue.pop();
		auto two = strQueue.pop();
		auto three = strQueue.pop();
		REQUIRE(2 == strQueue.size());
		auto four = strQueue.pop();
		auto five = strQueue.pop();
		REQUIRE(0 == strQueue.size());

		REQUIRE(one);
		REQUIRE(two);
		REQUIRE(three);
		REQUIRE(four);
		REQUIRE(five);

		REQUIRE_FALSE(strQueue.pop());
		REQUIRE_FALSE(strQueue.pop());
		REQUIRE_FALSE(strQueue.pop());

		REQUIRE("1" == *one);
		REQUIRE("2" == *two);
		REQUIRE("3" == *three);
		REQUIRE("4" == *four);
		REQUIRE("5" == *five);

	}

	SECTION("Mixte") {
		fys::common::SizedQueue<std::string> strQ;

		strQ.push("1");
		strQ.push("2");
		strQ.push("3");
		strQ.push("4");
		strQ.push("5");

		REQUIRE(5 == strQ.size());
		auto elem1 = strQ.pop();
		auto elem2 = strQ.pop();
		REQUIRE(3 == strQ.size());
		REQUIRE(elem1);
		REQUIRE("1" == *elem1);
		REQUIRE(elem2);
		REQUIRE("2" == *elem2);
		REQUIRE(strQ.push("pushing1"));
		REQUIRE(strQ.push("pushing2"));
		REQUIRE(strQ.push("pushing3"));
		REQUIRE(6 == strQ.size());
		REQUIRE(strQ.push("pushing4"));
		REQUIRE(strQ.push("pushing5"));
		REQUIRE(strQ.push("pushing6"));
		REQUIRE(9 == strQ.size());
		auto elem3 = strQ.pop();
		auto elem4 = strQ.pop();
		auto elem5 = strQ.pop();
		REQUIRE(6 == strQ.size());
		REQUIRE(elem3);
		REQUIRE("3" == *elem3);
		REQUIRE(elem4);
		REQUIRE("4" == *elem4);
		REQUIRE(elem5);
		REQUIRE("5" == *elem5);
		auto elem6 = strQ.pop();
		auto elem7 = strQ.pop();
		REQUIRE(4 == strQ.size());
		REQUIRE(elem6);
		REQUIRE("pushing1" == *elem6);
		REQUIRE(elem7);
		REQUIRE("pushing2" == *elem7);

		REQUIRE(strQ.pop());
		REQUIRE(3 == strQ.size());
		REQUIRE(strQ.pop());
		REQUIRE(2 == strQ.size());
		REQUIRE(strQ.pop());
		REQUIRE(1 == strQ.size());
		REQUIRE(strQ.pop());

		REQUIRE(0 == strQ.size());
		REQUIRE_FALSE(strQ.pop());
		REQUIRE(0 == strQ.size());

	} // End section : Mixte

} // End Case : SizedQueue testing
