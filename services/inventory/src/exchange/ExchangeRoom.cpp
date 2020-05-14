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

#include <exchange/ExchangeRoom.hh>

namespace fys::inv {

bool
ExchangeRoom::addItemFromExchangeForPlayer(const std::string& player, const std::string& token, ExchangeContent toAdd)
{
	if (!basicCheck(player, token)) return false;

	ExchangeRole role = getRolePlayerInExchangeRoom(player);

	if (role == ExchangeRole::NONE) {
		SPDLOG_ERROR("[Exchange : Room {}] : Player {} isn't part of the exchange.", player);
		return false;
	}

	auto& contentOfPlayer = _content[static_cast<uint>(role)];
	auto itemIt = std::find_if(contentOfPlayer.begin(), contentOfPlayer.end(),
			[&toAdd](const auto& c) { return c.itemCode == toAdd.itemCode; });

	if (itemIt == contentOfPlayer.end()) {
		toAdd.quantity += itemIt->quantity;
		// todo verify if the item exist in the given quantity
		itemIt->quantity = toAdd.quantity;
	}
	else {
		// todo verify if the item exist in the given quantity
		contentOfPlayer.emplace_back(std::move(toAdd));
	}
	return true;
}

bool
ExchangeRoom::removeItemFromExchangeForPlayer(const std::string& player, const std::string& token, const std::string& toRemove)
{
	if (!basicCheck(player, token)) return false;

	ExchangeRole role = getRolePlayerInExchangeRoom(player);

	if (role == ExchangeRole::NONE) {
		SPDLOG_ERROR("[Exchange : Room {}] : Player {} isn't part of the exchange.", _roomId, player);
		return false;
	}

	auto& contentOfPlayer = _content[static_cast<uint>(role)];
	auto itemIt = std::find_if(contentOfPlayer.begin(), contentOfPlayer.end(),
			[&toRemove](const auto& c) { return c.itemCode == toRemove; });

	if (itemIt == contentOfPlayer.end()) {
		contentOfPlayer.erase(itemIt);
	}
	else {
		SPDLOG_WARN("[Exchange : Room {}] : Player {} tried to remove object '{}' which isn't in the sharing room.",
				_roomId, player, toRemove);
		return false;
	}
	return true;
}

void
ExchangeRoom::lockExchange(const std::string& initiatorPlayer, const std::string& token)
{
	if (!basicCheck(initiatorPlayer, token)) return;
	if (_initiatorUserName != initiatorPlayer) {
		SPDLOG_ERROR("[Exchange : Room {}] : Player '{}' cant't lock exchange, initiator is '{}'.",
				_roomId, initiatorPlayer, _initiatorUserName);
		return;
	}
	_step = StepExchange::LOCKED_BY_INITIATOR;
}

void
ExchangeRoom::terminateExchange(const std::string& receiverPlayer, const std::string& token)
{
	if (!basicCheck(receiverPlayer, token)) return;
	if (_receiverUserName != receiverPlayer) {
		SPDLOG_ERROR("[Exchange : Room {}] : Player '{}' cant't validate exchange, receiver is '{}'.",
				_roomId, receiverPlayer, _receiverUserName);
		return;
	}
	// todo : Execute transaction
	_step = StepExchange::TERMINATED;
}

fys::inv::ExchangeRoom::ExchangeRole
inv::ExchangeRoom::getRolePlayerInExchangeRoom(const std::string& idPlayer) const noexcept
{
	if (_initiatorUserName == idPlayer) {
		return ExchangeRole::INITIATOR;
	}
	else if (_receiverUserName == idPlayer) {
		return ExchangeRole::RECEIVER;
	}
	return ExchangeRole::NONE;
}

bool
inv::ExchangeRoom::basicCheck(const std::string& player, const std::string& token)
{
	if (_step != StepExchange::ON_GOING) {
		SPDLOG_ERROR("[Exchange : Room {}] : Player '{}' action impossible, exchange room isn't on-going", _roomId, player);
		return false;
	}
	if (token != _tokenExchange) {
		SPDLOG_ERROR("[Exchange : Room {}] : Player {} didn't provide the good exchange token (provided {}).",
				_roomId, player, token);
		return false;
	}
	return true;
}

}