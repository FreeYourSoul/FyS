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

#ifndef FYS_WORLDSERVERENGINE_HH
#define FYS_WORLDSERVERENGINE_HH

#include <chrono>
#include <map>
#include <utility>

#include <zmq_addon.hpp>

#include <WSAction_generated.h>

#include "script_engine.hh"
#include <direct_connection_manager.hh>
#include <engine/collision_map.hh>
#include <engine/player_data.hh>
#include <engine/world_populator.hh>

// forward declaration
namespace fys::ws {
class script_engine;
}
namespace fys::fb {
struct WSAction;
}
// end forward declarations

namespace fys::ws {

constexpr static std::uint32_t NOT_AUTHENTICATED = std::numeric_limits<uint>::max();

struct auth_player {
  std::string user_name;
  std::string token;

  bool operator==(const auth_player& other) const {
    return user_name == other.user_name && token == other.token;
  }
  bool operator<(const auth_player& other) const {
    return user_name < other.user_name && token < other.token;
  }
};

class engine : public common::direct_connection_manager {

  friend class world_populator;

  struct internal;

public:
  explicit engine(const std::string& player_connect_str, collision_map&& map,
                  std::unique_ptr<script_engine> engine, std::chrono::system_clock::duration time_interval);
  ~engine();
  engine(engine&&) noexcept;
  engine(const engine&) = delete;

  void execute_pending_moves(const std::chrono::system_clock::time_point& current_time);
  void set_player_move_direction(std::uint32_t index, double direction);
  void stop_player_move(std::uint32_t index);
  void authenticate_player(auth_player auth, character_info info, std::string identifier);
  void spawnNPC(const std::chrono::system_clock::time_point& currentTime);

  [[nodiscard]] std::uint32_t
  retrieve_data_index(const auth_player& player);

private:
  inline void move_character_action(const std::string& character_name, std::uint32_t index_character, character_info& info);
  inline void notify_reported_npc_movements(const npc_actions_report& action_report);
  inline void notify_clients_of_character_move(const character_info& info, const std::string& user_name,
                                               const std::vector<std::string_view>& idts_to_identify);

private:
  std::unique_ptr<internal> _intern;

  //! Engine managing scripts of NPC and map triggers
  std::unique_ptr<script_engine> _script_engine;

  //! Authenticated user token to index in PlayersData
  std::map<auth_player, uint> _auth_player_on_data_index;
};

}// namespace fys::ws

#endif//FYS_WORLDSERVERENGINE_HH
