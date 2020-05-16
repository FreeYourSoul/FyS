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


#ifndef FYS_ONLINE_EXCHANGEMANAGER_HH
#define FYS_ONLINE_EXCHANGEMANAGER_HH

#include <map>
#include <DirectConnectionManager.hh>
#include <exchange/ExchangeRoom.hh>

// forward declarations
namespace fys::inv {
class InventoryServerContext;
class ItemManager;
}
// end forward declarations

namespace fys::inv {

class ExchangeManager : public common::DirectConnectionManager {

public:
	explicit ExchangeManager(const InventoryServerContext& ctx);

	void makeExchangeRoom(ItemManager& itemManager, const std::string& tokenBase);

private:
	[[nodiscard]] inline std::string
	generateToken(const std::string& tokenBase) const;

private:
	std::map<unsigned, ExchangeRoom> _rooms;
	const std::string_view _inventoryServerCode;
	unsigned _currentRoomId = 0;
};

}

#endif //FYS_ONLINE_EXCHANGEMANAGER_HH
