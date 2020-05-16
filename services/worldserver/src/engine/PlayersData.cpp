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
#include "engine/PlayersData.hh"

namespace fys::ws {

PlayersData::PlayersData(uint maxConnection) noexcept {
	_positions.reserve(maxConnection);
	_status.reserve(maxConnection);
	_identities.reserve(maxConnection);
	_userNames.reserve(maxConnection);
}

std::vector<std::string_view>
PlayersData::getPlayerIdtsAroundPlayer(uint indexPlayer,
		std::optional<std::reference_wrapper<PlayerInfo>> position,
		double distance) const noexcept
{
	if (position.has_value() && indexPlayer < _positions.size()) {
		return getPlayerIdtsAroundPos(position->get().pos, distance, indexPlayer);
	}
	return getPlayerIdtsAroundPos(_positions.at(indexPlayer).pos, distance, indexPlayer);
}

std::vector<std::string_view>
PlayersData::getPlayerIdtsAroundPos(const Pos& position,
		double distance,
		uint ignoreIndex) const noexcept
{
	std::vector<std::string_view> playerIdts;
	playerIdts.reserve(LIMIT_NOTIFICATIONS_MOVE);
	for (std::size_t i = 0; i < _positions.size(); ++i) {
		if (playerIdts.size() >= LIMIT_NOTIFICATIONS_MOVE) {
			break;
		}
		if (i == ignoreIndex) {
			continue;
		}
		Pos minimumPos = _positions.at(i).pos - distance;
		Pos maximumPos = _positions.at(i).pos + distance;

		if ((position.x > minimumPos.x && position.x < maximumPos.x) &&
				(position.y > minimumPos.y && position.x < maximumPos.y)) {
			playerIdts.emplace_back(_identities.at(i));
		}
	}
	playerIdts.shrink_to_fit();
	return playerIdts;
}

uint
PlayersData::addNewPlayerData(PlayerInfo info, std::string identity, std::string userName)
{
	if (!(_identities.size() == _status.size() == _positions.size() == _userNames.size())) {
		SPDLOG_CRITICAL("MISMATCH, all vectors require to be equal idt:'{}' stat:'{}' pos:'{}', names:'{}'",
				_identities.size(), _status.size(), _positions.size(), _userNames.size());
		return 0;
	}
	uint index = _identities.size();
	_positions.emplace_back(std::move(info));
	_status.emplace_back(PlayerStatus::STANDING);
	_identities.emplace_back(std::move(identity));
	_userNames.emplace_back(std::move(userName));
	return index;
}

}
