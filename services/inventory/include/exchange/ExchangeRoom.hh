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


#ifndef FYS_ONLINE_EXCHANGEROOM_HH
#define FYS_ONLINE_EXCHANGEROOM_HH

#include <string>
#include <array>
#include <vector>
#include <ItemManager.hh>

namespace fys::inv {

enum class StepExchange {
	// The exchange is currently on-going and addition, or deletion of item is accepted
	ON_GOING,

	// The exchange is blocked, and the validation has to be made, otherwise it return to ON_GOING
	LOCKED_BY_INITIATOR,

	// Exchange is finished, the room can be cleaned up
	TERMINATED
};

class ExchangeRoom {

	enum class ExchangeRole : uint {
		INITIATOR = 0,
		RECEIVER = 1,
		NONE
	};

public:
	explicit ExchangeRoom(uint roomId, std::string tokenExchange, ItemManager& refIm)
			:_roomId(roomId), _tokenExchange(std::move(tokenExchange)), _manager(refIm) { }

	[[nodiscard]] bool
	addItemFromExchangeForPlayer(const std::string& player, const std::string& token, Item toAdd);

	[[nodiscard]] bool
	removeItemFromExchangeForPlayer(const std::string& player, const std::string& token, const std::string& toRemove);

	[[nodiscard]] StepExchange
	getCurrentStep() const { return _step; }

	void lockExchange(const std::string& initiatorPlayer, const std::string& token);
	void terminateExchange(const std::string& receiverPlayer, const std::string& token);

private:
	[[nodiscard]] inline ExchangeRole
	getRolePlayerInExchangeRoom(const std::string& idPlayer) const noexcept;

	[[nodiscard]] inline bool
	basicCheck(const std::string& player, const std::string& token);

private:
	uint _roomId;
	std::string _tokenExchange;
	std::reference_wrapper<ItemManager> _manager;

	StepExchange _step = StepExchange::ON_GOING;

	std::string _initiatorUserName;
	std::string _receiverUserName;

	std::array<std::vector<Item>, 2> _content;

};

}

#endif //FYS_ONLINE_EXCHANGEROOM_HH
