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

#include <spdlog/spdlog.h>

#include <item_manager.hh>

#include <exchange/exchange_room.hh>

namespace fys::inv {

bool exchange_room::addItemFromExchangeForPlayer(const std::string& player, const std::string& token, Item toAdd) {
  if (!basicCheck(player, token) || toAdd.quantity == 0)
	return false;

  ExchangeRole role = getRolePlayerInExchangeRoom(player);

  if (role == ExchangeRole::NONE) {
	SPDLOG_ERROR("[Exchange : Room {}] : Player {} isn't part of the exchange.", _roomId, player);
	return false;
  }

  auto& contentOfPlayer = _content[static_cast<uint>(role)];
  auto itemIt = std::find_if(contentOfPlayer.begin(), contentOfPlayer.end(),
							 [&toAdd](const auto& c) { return c.itemCode == toAdd.itemCode; });

  if (itemIt != contentOfPlayer.end()) {
	toAdd.quantity += itemIt->quantity;
	if (_manager.get().are_items_owned_by_user(player, {toAdd})) {
	  SPDLOG_ERROR("[Exchange : Room {}] : Player {} doesn't have item [code : {}, quantity : {}].",
				   _roomId, player, toAdd.itemCode, toAdd.quantity);
	  return false;
	}
	itemIt->quantity = toAdd.quantity;

  } else {
	if (_manager.get().are_items_owned_by_user(player, {toAdd})) {
	  SPDLOG_ERROR("[Exchange : Room {}] : Player {} doesn't have item [code : {}, quantity : {}].",
				   _roomId, player, toAdd.itemCode, toAdd.quantity);
	  return false;
	}
	contentOfPlayer.emplace_back(toAdd);
  }
  return true;
}

bool exchange_room::removeItemFromExchangeForPlayer(const std::string& player, const std::string& token, const Item& toRemove) {
  if (!basicCheck(player, token))
	return false;

  ExchangeRole role = getRolePlayerInExchangeRoom(player);

  if (role == ExchangeRole::NONE) {
	SPDLOG_ERROR("[Exchange : Room {}] : Player {} isn't part of the exchange.", _roomId, player);
	return false;
  }

  auto& contentOfPlayer = _content[static_cast<uint>(role)];
  auto itemIt = std::find_if(contentOfPlayer.begin(), contentOfPlayer.end(),
							 [&toRemove](const auto& c) { return c.itemCode == toRemove.itemCode; });

  if (itemIt != contentOfPlayer.end()) {
	if (toRemove.quantity >= itemIt->quantity) {
	  contentOfPlayer.erase(itemIt);
	} else {
	  itemIt->quantity -= toRemove.quantity;
	}
  } else {
	SPDLOG_WARN("[Exchange : Room {}] : Player {} tried to remove object '{}' which isn't in the sharing room.",
				_roomId, player, toRemove.itemCode);
	return false;
  }
  return true;
}

bool exchange_room::lockExchange(const std::string& initiatorPlayer, const std::string& token) {
  if (!basicCheck(initiatorPlayer, token))
	return false;
  if (_initiatorUserName != initiatorPlayer) {
	SPDLOG_ERROR("[Exchange : Room {}] : Player '{}' cant't lock exchange, initiator is '{}'.",
				 _roomId, initiatorPlayer, _initiatorUserName);
	return false;
  }
  _step = StepExchange::LOCKED_BY_INITIATOR;
  return true;
}

bool exchange_room::terminateExchange(const std::string& receiverPlayer, const std::string& token) {
  if (!basicCheck(receiverPlayer, token))
	return false;
  if (_receiverUserName != receiverPlayer) {
	SPDLOG_ERROR("[Exchange : Room {}] : Player '{}' cant't validate exchange, receiver is '{}'.",
				 _roomId, receiverPlayer, _receiverUserName);
	return false;
  }
  // todo : Execute transaction
  _step = StepExchange::TERMINATED;
  return true;
}

bool exchange_room::receiverJoin(const std::string& receiver, const std::string& token, std::string identity) {
  if (_step != StepExchange::AWAIT_TO_BE_JOINED_BY_RECEIVER) {
	SPDLOG_ERROR("[Exchange : Room {}] : Player '{}' action impossible, exchange room isn't on-going", _roomId, receiver);
	return false;
  }
  if (token != _tokenExchange) {
	SPDLOG_ERROR("[Exchange : Room {}] : Player '{}' didn't provide the good exchange token (provided {}).",
				 _roomId, receiver, token);
	return false;
  }
  if (_receiverUserName != receiver) {
	SPDLOG_ERROR("[Exchange : Room {}] : Player '{}' cant't join because isn't the receiver, receiver is '{}'.",
				 _roomId, receiver, _receiverUserName);
	return false;
  }
  _receiverIdentity = std::move(identity);
  _step = StepExchange::ON_GOING;
  return true;
}

bool inv::exchange_room::basicCheck(const std::string& player, const std::string& token) {
  if (_step != StepExchange::ON_GOING) {
	SPDLOG_ERROR("[Exchange : Room {}] : Player '{}' action impossible, exchange room isn't on-going", _roomId, player);
	return false;
  }
  if (token != _tokenExchange) {
	SPDLOG_ERROR("[Exchange : Room {}] : Player '{}' didn't provide the good exchange token (provided {}).",
				 _roomId, player, token);
	return false;
  }
  if (player != _initiatorUserName && player != _receiverUserName) {
	SPDLOG_ERROR("[Exchange : Room {}] : Player '{}' isn't the part of the transaction (awaited '{}' or '{}').",
				 _roomId, player, _initiatorUserName, _receiverUserName);
	return false;
  }
  return true;
}

fys::inv::exchange_room::ExchangeRole
inv::exchange_room::getRolePlayerInExchangeRoom(const std::string& idPlayer) const noexcept {
  if (_initiatorUserName == idPlayer) {
	return ExchangeRole::INITIATOR;
  } else if (_receiverUserName == idPlayer) {
	return ExchangeRole::RECEIVER;
  }
  return ExchangeRole::NONE;
}

}// namespace fys::inv