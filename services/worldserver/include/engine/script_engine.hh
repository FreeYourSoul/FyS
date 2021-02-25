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

#ifndef FYS_ONLINE_SCRIPT_ENGINE_HH
#define FYS_ONLINE_SCRIPT_ENGINE_HH

#include <CmlScriptDownloader.hh>
#include <chrono>
#include <engine/player_data.hh>
#include <engine/world_populator.hh>
#include <memory>

using namespace std::chrono_literals;

namespace fys::ws {

//! Spawning interval set by default for a given spawning point
static constexpr std::chrono::seconds BASE_SPAWNING_INTERVAL = 40s;

/**
 * A Spawning point is a specific area in the world that is scripted to spawn encounters that are going to move around
 * and re-spawn if defeated.
 * The spawning behavior is scripted in the LUA Script, and the encounters script is defined by this Spawning point script
 */
struct spawning_point {

  //! Approximate Center of the spawning point, used in conjunction with distanceNotification
  pos center_spawning_point = {0.0, 0.0};
  //! Distance from the centerSpawningPoint at which a player get notified of a monster move
  double distance_notification;

  unsigned max_spawned = 0;
  std::string id_spawning_point;
  std::string display_key;

  std::chrono::seconds spawning_interval = BASE_SPAWNING_INTERVAL;
  std::chrono::system_clock::time_point next_spawn{};
};

/**
 * Each action made by an NPC via LUA script has to generate an actionId representing the action made
 * If this id is equal to '1' or '2'. A movement has been made or stopped and afterMove is filled.
 * This struct is used in order to generate the notifications to send to the players
 */
struct npc_action {
  //! id defining a idle action
  static constexpr std::uint32_t IDLE = 0u;
  //! id defining a move action
  static constexpr std::uint32_t MOVE = 1u;
  //! id defining a stop move action
  static constexpr std::uint32_t STOP = 2u;

  //! Id defining the npc in LUA engine
  std::uint32_t npc_lua_id = 0u;

  //! Spawning point id linked to the encounter npc, std::nullopt if it's a neutral
  std::optional<std::string_view> id_spawning_point;

  /**
   * Id of the action made by the NPC
   * 0 means no action are made (no notification should be sent)
   * 1 means a move action is made (afterMove must be filled in this case)
   * 2 means a move stopped (afterMove must be filled in this case)
   * Another id is a specific action (animation..)
   */
  std::uint32_t action_id = IDLE;
  character_info after_move{};
};

/**
 * @brief npc actions report of a server tick
 *
 * Every element are index related
 * (ex: central_position[0] with notification_distance[0] is related to the vector of npc_action npc_actions[0])
 */
struct npc_actions_report {

  /**
   * Position
   */
  std::vector<pos> central_positions;
  std::vector<double> notification_distances;
  std::vector<std::vector<npc_action>> npc_actions;
};

class script_engine {
  friend class world_populator;

  struct internal;

  //! character_info_ret_type : position x, position y, velocity, angle
  using character_info_ret_type = std::tuple<double, double, double, double>;

public:
  script_engine();
  ~script_engine();

  void spawn_new_encounters(const std::chrono::system_clock::time_point& current_time);

  /**
   * @brief execute all scripted actions and return a report
   * @details Execute neutral and enemy npc (non playable character) scripted actions, merge both npc report
   * and return it.
   * Neutral and Aggressive are distinguished because
   * @return Report of all the actions executed by scripted entities
   */
  [[nodiscard]] npc_actions_report execute_scripted_actions();

private:
  std::unique_ptr<internal> _intern;
};

}// namespace fys::ws

#endif//FYS_ONLINE_SCRIPT_ENGINE_HH
