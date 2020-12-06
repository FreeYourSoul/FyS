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

  /**
   * @brief Set to true if the responsibility of the awaited player is for the current instance of the server
   *
   * @details An awaited player can come directly from authentication (responsibility to true) or from a
   * server to server transition (responsibility to false until the server from give up this responsibility)
   *
   * A non-responsible player when connected is invisible for the other player of this server
   */
  bool is_responsible = true;

  bool operator==(const auth_player& other) const {
    return user_name == other.user_name && token == other.token;
cd  bool operator<(const auth_player& other) const {
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

  /**
   * @brief execute the pending movement that has been registered for each player if the provided ticking time is late enough
   * @param current_time ticking time to determine if movement has to happen
   */
  void execute_pending_moves(const std::chrono::system_clock::time_point& current_time);

  /**
   * @brief notification from a neighbour server in order to keep the soon to be incoming player position and responsibility
   * @param transition_update information of the player to update
   * @return true if the update happened (if authenticated and currently not under server responsibility), false otherwise
   */
  bool server_transition_update(const auth_player& transition_update);

  /**
   * @brief do a moving action for the given player
   * @param index of the player to do a movement on
   * @param direction to move to
   */
  void set_player_move_direction(std::uint32_t index, double direction);

  /**
   * @brief stop the moving action for the given player
   * @param index player index to stop the movement of
   */
  void stop_player_move(std::uint32_t index);

  /**
   * @brief add a player to the authenticated list
   *
   * An authentication happens when a player that is previously awaited (because of server transition or initial connection)
   * is connecting to the server.
   *
   * @param auth authentication info
   * @param info character information
   * @param identifier used to identify the authenticated player in order to send him messages
   */
  void authenticate_player(auth_player auth, character_info info, std::string identifier);

  /**
   * @brief launch the different lua scripted spawning process
   * @param current_time timestamp of the current loop tick (potentially used by the spawning process script)
   */
  void spawnNPC(const std::chrono::system_clock::time_point& currentTime);

  /**
   * @brief retrieve the index of an active, authenticated player
   *
   * The index is used in order to access user data (all data are indexed specifically for a given player)
   * An active player is a player the current instance of the server is responsible of.
   *
   * @param player player info to get the index of
   * @return fys::ws::NOT_AUTHENTICATED is returned if the player isn't authenticated or active
   */
  [[nodiscard]] std::uint32_t retrieve_data_index(const auth_player& player) noexcept;

private:
  inline void move_character_action(const std::string& character_name,
                                    std::uint32_t index_character, character_info& info);

  inline void notify_reported_npc_movements(const npc_actions_report& action_report);

  inline void notify_clients_of_character_move(const character_info& info, const std::string& user_name,
                                               const std::vector<std::string_view>& idts_to_identify);

private:
  std::unique_ptr<internal> _intern;

  //! Engine managing scripts of NPC and map triggers
  std::unique_ptr<script_engine> _script_engine;

  //! Authenticated user token to index in PlayersData
  std::map<auth_player, std::uint32_t> _auth_player_on_data_index;
};

}// namespace fys::ws

#endif//FYS_WORLDSERVERENGINE_HH
