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

#ifndef FYS_ONLINE_EXCHANGE_MANAGER_HH
#define FYS_ONLINE_EXCHANGE_MANAGER_HH

#include <map>
#include <direct_connection_manager.hh>
#include <exchange/exchange_room.hh>

// forward declarations
namespace fys::inv {
class inventory_server_context;
class item_manager;
}
// end forward declarations

namespace fys::inv {

struct room_accessor {
	uint idExchange;
	std::string userName;
	std::string tokenExchange;
};

class exchange_manager : public common::direct_connection_manager {

public:
	explicit exchange_manager(const inventory_server_context& ctx);

	[[nodiscard]] const exchange_room&
	makeExchangeRoom(item_manager& itemManager, std::string initiator, std::string receiver, std::string identity);

	void lock_room_transaction(const room_accessor& accessor);
	void receiver_join_room_transaction(const room_accessor& accessor, std::string identity_receiver);
	void terminate_room_transaction(item_manager& manager, const room_accessor& accessor);
	void remove_item_from_room(item_manager& manager, const room_accessor& accessor, const std::string& item_code, uint qty);
	void add_item_in_room(item_manager& manager, const room_accessor& accessor, const std::string& item_code, uint qty);

private:
	[[nodiscard]] inline std::string
	generate_token(const std::string& token_base) const;

private:
	std::map<unsigned, exchange_room> _rooms;
	const std::string_view _inventory_server_code;
	unsigned _current_room_id = 0;
};

}

#endif //FYS_ONLINE_EXCHANGE_MANAGER_HH
