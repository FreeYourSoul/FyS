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

#include <exchange/ExchangeRoom.hh>

static const std::string ACTUAL_TOKEN = "ThisIsTheActualTokenOfTheExchangeRoom";
static const std::string RECEIVER = "ReceiverName";
static const std::string INITIATOR = "InitiatorName";
static constexpr uint EXCHANGE_ID = 42;

TEST_CASE("ExchangeRoomTestCase", "[service][inventory]")
{
	fys::inv::ItemManager im;
	fys::inv::ExchangeRoom exchangeRoom(EXCHANGE_ID, INITIATOR, RECEIVER, ACTUAL_TOKEN, im);

	SECTION("addItemFromExchangeForPlayer test") {

	} // End section : addItemFromExchangeForPlayer test

	SECTION("removeItemFromExchangeForPlayer test") {

	} // End section : removeItemFromExchangeForPlayer test

} // End TestCase : ExchangeRoomTestCase