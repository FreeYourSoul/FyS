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

#include <exchange/exchange_room.hh>

static const std::string ACTUAL_TOKEN = "ThisIsTheActualTokenOfTheExchangeRoom";
static const std::string RECEIVER = "ReceiverName";
static const std::string RECEIVER_IDENTITY = "ReceiverIdt";
static const std::string INITIATOR = "InitiatorName";
static const std::string INITIATOR_IDENTITY = "InitiatorIdt";
static constexpr std::uint32_tEXCHANGE_ID = 42;

using namespace fys::inv;

TEST_CASE("ExchangeRoomTestCase", "[service][inventory]") {
  item_manager im;
  exchange_room exchangeRoom(EXCHANGE_ID, INITIATOR, RECEIVER, INITIATOR_IDENTITY, ACTUAL_TOKEN, im);

  const auto &initiatorContent = exchangeRoom.getInitiatorContent();
  const auto &receiverContent = exchangeRoom.getReceiverContent();

  REQUIRE(exchangeRoom.getInitiatorContent().empty());
  REQUIRE(exchangeRoom.getReceiverContent().empty());

  REQUIRE(StepExchange::AWAIT_TO_BE_JOINED_BY_RECEIVER == exchangeRoom.getCurrentStep());

  SECTION("receiverJoin test") {
	// Not ON-GOING
	REQUIRE(StepExchange::AWAIT_TO_BE_JOINED_BY_RECEIVER == exchangeRoom.getCurrentStep());
	REQUIRE_FALSE(exchangeRoom.addItemFromExchangeForPlayer(INITIATOR, ACTUAL_TOKEN, Item{"ItemCode", 2}));
	REQUIRE(exchangeRoom.getInitiatorContent().empty());

	REQUIRE_FALSE(exchangeRoom.receiverJoin(RECEIVER, "NOT_TOKEN", RECEIVER_IDENTITY));
	REQUIRE(StepExchange::AWAIT_TO_BE_JOINED_BY_RECEIVER == exchangeRoom.getCurrentStep());
	REQUIRE_FALSE(exchangeRoom.addItemFromExchangeForPlayer(INITIATOR, ACTUAL_TOKEN, Item{"ItemCode", 2}));
	REQUIRE(exchangeRoom.getInitiatorContent().empty());

	REQUIRE(exchangeRoom.receiverJoin(RECEIVER, ACTUAL_TOKEN, RECEIVER_IDENTITY));
	REQUIRE(StepExchange::ON_GOING == exchangeRoom.getCurrentStep());
	REQUIRE(exchangeRoom.addItemFromExchangeForPlayer(INITIATOR, ACTUAL_TOKEN, Item{"ItemCode", 2}));
	REQUIRE_FALSE(exchangeRoom.getInitiatorContent().empty());

  }// End section : receiverJoin test

  SECTION("addItemFromExchangeForPlayer test") {
	REQUIRE(exchangeRoom.receiverJoin(RECEIVER, ACTUAL_TOKEN, RECEIVER_IDENTITY));
	REQUIRE(StepExchange::ON_GOING == exchangeRoom.getCurrentStep());
	REQUIRE(exchangeRoom.addItemFromExchangeForPlayer(INITIATOR, ACTUAL_TOKEN, Item{"ItemCode", 2}));

	REQUIRE_FALSE(initiatorContent.empty());
	REQUIRE(exchangeRoom.getReceiverContent().empty());
	REQUIRE(1 == initiatorContent.size());
	REQUIRE("ItemCode" == initiatorContent.at(0).itemCode);
	REQUIRE(2 == initiatorContent.at(0).quantity);

	REQUIRE(exchangeRoom.addItemFromExchangeForPlayer(INITIATOR, ACTUAL_TOKEN, Item{"SecondItem", 6}));
	REQUIRE(exchangeRoom.addItemFromExchangeForPlayer(INITIATOR, ACTUAL_TOKEN, Item{"ItemCode", 2}));
	REQUIRE(exchangeRoom.addItemFromExchangeForPlayer(RECEIVER, ACTUAL_TOKEN, Item{"Money", 42}));

	REQUIRE_FALSE(initiatorContent.empty());
	REQUIRE_FALSE(exchangeRoom.getReceiverContent().empty());

	REQUIRE(2 == initiatorContent.size());
	REQUIRE("ItemCode" == initiatorContent.at(0).itemCode);
	REQUIRE(4 == initiatorContent.at(0).quantity);
	REQUIRE("SecondItem" == initiatorContent.at(1).itemCode);
	REQUIRE(6 == initiatorContent.at(1).quantity);

	REQUIRE(1 == receiverContent.size());
	REQUIRE("Money" == receiverContent.at(0).itemCode);
	REQUIRE(42 == receiverContent.at(0).quantity);

  }// End section : addItemFromExchangeForPlayer test

  SECTION("addItemFromExchangeForPlayer Error testing") {
	REQUIRE(exchangeRoom.receiverJoin(RECEIVER, ACTUAL_TOKEN, RECEIVER_IDENTITY));
	REQUIRE_FALSE(exchangeRoom.addItemFromExchangeForPlayer(INITIATOR, "NOT_TOKEN", Item{"ItemCode", 2}));

	// verify that content is still empty
	REQUIRE(exchangeRoom.getInitiatorContent().empty());
	REQUIRE(exchangeRoom.getReceiverContent().empty());

	REQUIRE_FALSE(exchangeRoom.addItemFromExchangeForPlayer(INITIATOR, "NOT_TOKEN", Item{"ItemCode", 0}));

	// verify that content is still empty
	REQUIRE(exchangeRoom.getInitiatorContent().empty());
	REQUIRE(exchangeRoom.getReceiverContent().empty());

	REQUIRE_FALSE(exchangeRoom.addItemFromExchangeForPlayer("NotPlayer", ACTUAL_TOKEN, Item{"ItemCode", 2}));

	// verify that content is still empty
	REQUIRE(exchangeRoom.getInitiatorContent().empty());
	REQUIRE(exchangeRoom.getReceiverContent().empty());

	REQUIRE(exchangeRoom.addItemFromExchangeForPlayer(INITIATOR, ACTUAL_TOKEN, Item{"ItemCode", 2}));
	REQUIRE_FALSE(exchangeRoom.getInitiatorContent().empty());
	REQUIRE(exchangeRoom.getReceiverContent().empty());

  }// End section : Error testing

  SECTION("removeItemFromExchangeForPlayer test") {
	REQUIRE(exchangeRoom.receiverJoin(RECEIVER, ACTUAL_TOKEN, RECEIVER_IDENTITY));
	REQUIRE(exchangeRoom.addItemFromExchangeForPlayer(INITIATOR, ACTUAL_TOKEN, Item{"SecondItem", 6}));
	REQUIRE(exchangeRoom.addItemFromExchangeForPlayer(INITIATOR, ACTUAL_TOKEN, Item{"ItemCode", 2}));
	REQUIRE(exchangeRoom.addItemFromExchangeForPlayer(RECEIVER, ACTUAL_TOKEN, Item{"Money", 42}));

	REQUIRE(2 == initiatorContent.size());
	REQUIRE(1 == receiverContent.size());

	REQUIRE(exchangeRoom.removeItemFromExchangeForPlayer(INITIATOR, ACTUAL_TOKEN, Item{"ItemCode", 2}));
	// ItemCode is removed
	REQUIRE(1 == initiatorContent.size());
	REQUIRE(1 == receiverContent.size());

	// Over kill remove
	REQUIRE(exchangeRoom.removeItemFromExchangeForPlayer(INITIATOR, ACTUAL_TOKEN, Item{"SecondItem", 1337}));
	REQUIRE(initiatorContent.empty());

	// Partial remove
	REQUIRE(exchangeRoom.removeItemFromExchangeForPlayer(RECEIVER, ACTUAL_TOKEN, Item{"Money", 20}));
	REQUIRE(initiatorContent.empty());
	REQUIRE(1 == receiverContent.size());
	REQUIRE(22 == receiverContent.at(0).quantity);

	// test can't remove with wrong token
	REQUIRE_FALSE(exchangeRoom.removeItemFromExchangeForPlayer(RECEIVER, "WRONG_TOKEN", Item{"Money", 20}));
	REQUIRE(initiatorContent.empty());
	REQUIRE(1 == receiverContent.size());
	REQUIRE(22 == receiverContent.at(0).quantity);

	// test can't remove with wrong username
	REQUIRE_FALSE(exchangeRoom.removeItemFromExchangeForPlayer("WrongUserName", ACTUAL_TOKEN, Item{"Money", 20}));
	REQUIRE(initiatorContent.empty());
	REQUIRE(1 == receiverContent.size());
	REQUIRE(22 == receiverContent.at(0).quantity);

  }// End section : removeItemFromExchangeForPlayer test

  SECTION("lockExchange test") {
	REQUIRE(exchangeRoom.receiverJoin(RECEIVER, ACTUAL_TOKEN, RECEIVER_IDENTITY));
	REQUIRE(exchangeRoom.addItemFromExchangeForPlayer(INITIATOR, ACTUAL_TOKEN, Item{"ItemCode", 2}));
	REQUIRE(exchangeRoom.addItemFromExchangeForPlayer(RECEIVER, ACTUAL_TOKEN, Item{"Money", 42}));
	REQUIRE(1 == initiatorContent.size());
	REQUIRE(1 == receiverContent.size());

	// can't lock as receiver
	REQUIRE(StepExchange::ON_GOING == exchangeRoom.getCurrentStep());
	REQUIRE_FALSE(exchangeRoom.lockExchange(RECEIVER, ACTUAL_TOKEN));
	REQUIRE(StepExchange::ON_GOING == exchangeRoom.getCurrentStep());

	// lock by initiator
	REQUIRE(StepExchange::ON_GOING == exchangeRoom.getCurrentStep());
	REQUIRE(exchangeRoom.lockExchange(INITIATOR, ACTUAL_TOKEN));
	REQUIRE(StepExchange::LOCKED_BY_INITIATOR == exchangeRoom.getCurrentStep());

	// can't add or remove element
	REQUIRE_FALSE(exchangeRoom.addItemFromExchangeForPlayer(INITIATOR, ACTUAL_TOKEN, Item{"NOTADDED", 2}));
	REQUIRE_FALSE(exchangeRoom.addItemFromExchangeForPlayer(RECEIVER, ACTUAL_TOKEN, Item{"NOTADDED", 42}));
	// verify no added element
	REQUIRE(1 == initiatorContent.size());
	REQUIRE(1 == receiverContent.size());

	// Overkill remove attempt fail while locked
	REQUIRE_FALSE(exchangeRoom.removeItemFromExchangeForPlayer(INITIATOR, ACTUAL_TOKEN, Item{"ItemCode", 1337}));
	REQUIRE_FALSE(exchangeRoom.removeItemFromExchangeForPlayer(RECEIVER, ACTUAL_TOKEN, Item{"Money", 1337}));
	// verify no removed element
	REQUIRE(1 == initiatorContent.size());
	REQUIRE(1 == receiverContent.size());

  }// End section : lockExchange test

}// End TestCase : ExchangeRoomTestCase