// MIT License
//
// Copyright (c) 2021 Quentin Balland
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

#ifndef FYS_ONLINE_INVENTORY_SERVER_SERVICE_HH
#define FYS_ONLINE_INVENTORY_SERVER_SERVICE_HH

#include <connection_handler.hh>
#include <exchange/exchange_manager.hh>
#include <utility>

// forward declarations
namespace fys::fb::ivt {
struct update_player_soul_draughtboard;
struct RetrievePlayerSoulDraughtboard;
struct retrieve_player_inventory;
struct initiate_exchange_instance;
}// namespace fys::fb::ivt
namespace fys::inv {
class inventory_server_context;
}
// end forward declarations

namespace fys::inv {

class inventory_server_service {

public:
  inventory_server_service(const inventory_server_context& ctx);

  void run_server_loop();

private:
  [[nodiscard]] zmq::message_t
  exchange_inventory(const fb::ivt::initiate_exchange_instance* exchange_request, std::string identity);

  [[nodiscard]] zmq::message_t
  update_player_soul_draughtboard(const fb::ivt::update_player_soul_draughtboard* update_request);

  [[nodiscard]] zmq::message_t
  retrievePlayerSoulDraughtboard(const fb::ivt::update_player_soul_draughtboard* update_request);

  [[nodiscard]] zmq::message_t
  retrieve_player_inventory(const fb::ivt::retrieve_player_inventory* retrieve_request);

private:
  std::reference_wrapper<const inventory_server_context> _ctx;
  exchange_manager _exchange_manager;
  item_manager _item_manager;
  common::connection_handler _connection_handler;
};

}// namespace fys::inv
#endif//FYS_ONLINE_INVENTORY_SERVER_SERVICE_HH
