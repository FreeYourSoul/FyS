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

#ifndef FYS_WORLDSERVERSERVICE_HH
#define FYS_WORLDSERVERSERVICE_HH

#include <engine/engine.hh>

#include "connection_handler.hh"

// forward declarations
namespace fys::ws {
class world_server_context;
}
namespace fys::fb::world {
struct WSAction;
struct InterServerComMove;
}// namespace fys::fb::world
// end forward declarations

namespace fys::ws {

class world_server_service {

  struct awaited_player {
    auth_player auth{};
    pos initial_position{};
    double initial_angle = 0.0;
    double initial_velocity = 0.0;
  };

public:
  explicit world_server_service(const world_server_context& ctx, engine engine);
  void run_server_loop() noexcept;

private:
  inline void register_awaited_player(const std::string& user, const std::string& token, std::string identity);
  inline void process_player_message(const std::string& user, const std::string& tkn, const fb::world::WSAction* action);

  /**
   * @brief handle transition movement initiated by another server.
   *
   * register the player as an awaited player without taking responsibility for him. if already registered
   * (but player didn't connect yet), update the initial position of the awaited player.
   *
   * @param move_transition object containing position, authentication and responsibility information for the player.
   */
  inline void process_transition_server_movement(const fb::world::InterServerComMove* move_transition);

private:
  std::reference_wrapper<const world_server_context> _ctx;

  std::vector<awaited_player> _awaited_incoming_player;

  engine _world_server;
  ws::connection_handler _connection_handler;
};

}// namespace fys::ws

#endif//FYS_WORLDSERVERSERVICE_HH
