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

#ifndef FYS_ONLINE_EXCHANGE_ROOM_HH
#define FYS_ONLINE_EXCHANGE_ROOM_HH

#include <array>
#include <item_manager.hh>
#include <string>
#include <vector>

namespace fys::inv {

enum class StepExchange {

  // Initial state set, the transaction start when the receiver register
  AWAIT_TO_BE_JOINED_BY_RECEIVER,

  // The exchange is currently on-going and addition, or deletion of item is accepted
  ON_GOING,

  // The exchange is blocked, and the validation has to be made, otherwise it return to ON_GOING
  LOCKED_BY_INITIATOR,

  // Exchange is finished, the room can be cleaned up
  TERMINATED

};

class exchange_room {

  enum class ExchangeRole : std::uint32_t {
	INITIATOR = 0,
	RECEIVER = 1,
	NONE
  };

public:
  explicit exchange_room(
	  std::uint32_t roomId,
	  std::string initiator,
	  std::string receiver,
	  std::string initiatorIdentity,
	  std::string tokenExchange,
	  item_manager& refIm)
	  : _roomId(roomId),
		_tokenExchange(std::move(tokenExchange)),
		_manager(refIm),
		_initiatorUserName(std::move(initiator)),
		_receiverUserName(std::move(receiver)),
		_initiatorIdentity(std::move(initiatorIdentity)) {}

  bool receiverJoin(const std::string& receiver, const std::string& token, std::string identity);
  bool addItemFromExchangeForPlayer(const std::string& player, const std::string& token, Item toAdd);
  bool removeItemFromExchangeForPlayer(const std::string& player, const std::string& token, const Item& toRemove);
  bool lockExchange(const std::string& initiatorPlayer, const std::string& token);
  bool terminateExchange(const std::string& receiverPlayer, const std::string& token);

  [[nodiscard]] StepExchange
  getCurrentStep() const { return _step; }

  [[nodiscard]] const std::vector<Item>&
  getInitiatorContent() const { return _content.at(static_cast<uint>(ExchangeRole::INITIATOR)); }

  [[nodiscard]] const std::vector<Item>&
  getReceiverContent() const { return _content.at(static_cast<uint>(ExchangeRole::RECEIVER)); }

private:
  [[nodiscard]] inline ExchangeRole
  getRolePlayerInExchangeRoom(const std::string& idPlayer) const noexcept;

  [[nodiscard]] inline bool
  basicCheck(const std::string& player, const std::string& token);

private:
  std::uint32_t _roomId;
  std::string _tokenExchange;
  std::reference_wrapper<item_manager> _manager;

  std::string _initiatorUserName;
  std::string _receiverUserName;

  std::string _initiatorIdentity;
  std::string _receiverIdentity;

  StepExchange _step = StepExchange::AWAIT_TO_BE_JOINED_BY_RECEIVER;

  std::array<std::vector<Item>, 2> _content;
};

}// namespace fys::inv

#endif//FYS_ONLINE_EXCHANGE_ROOM_HH
