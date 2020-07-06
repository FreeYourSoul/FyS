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


#ifndef FYS_ONLINE_SCRIPT_ENGINE_HH
#define FYS_ONLINE_SCRIPT_ENGINE_HH

#include <memory>
#include <chrono>
#include <sol/sol.hpp>
#include <engine/world_populator.hh>
#include <engine/player_data.hh>
#include <CmlScriptDownloader.hh>

using namespace std::chrono_literals;

// forward declarations
namespace fys::ws {
class world_server_context;
}
// end forward declarations

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
 * A representation of a NPC instance (NPC can be a neutral npc or an encounter)
 */
struct npc_lua_instance {
	character_info info;

	//! Spawning Point namespace, empty in case of neutral encounter
	std::string spNamespace;

	/**
	 * Id of the npc used to reference the NPC in LUA
	 * In case of encounter, this will represent the index of the NPC in the spawning point NPC list
	 * In case of neutral NPC, this represent the unique id of the NPC
	 */
	uint npcLuaId;
};

/**
 * Each action made by an NPC via LUA script has to generate an actionId representing the action made
 * If this id is equal to '1' or '2'. A movement has been made or stopped and afterMove is filled.
 * This struct is used in order to generate the notifications to send to the players
 */
struct npc_action {
	//! id defining a idle action
	static constexpr uint IDLE = 0u;
	//! id defining a move action
	static constexpr uint MOVE = 1u;
	//! id defining a stop move action
	static constexpr uint STOP = 2u;

	//! Id defining the npc in LUA engine
	uint npc_lua_id = 0u;

	/**
	 * Id of the action made by the NPC
	 * 0 means no action are made (no notification should be sent)
	 * 1 means a move action is made (afterMove must be filled in this case)
	 * 2 means a move stopped (afterMove must be filled in this case)
	 * Another id is a specific action (animation..)
	 */
	uint actionId = IDLE;
	character_info afterMove {};
};

class script_engine {

	friend class world_populator;

	//! character_info_ret_type : position x, position y, velocity, angle
	using character_info_ret_type = std::tuple<double, double, double, double>;

public:
	void spawn_new_encounters(const std::chrono::system_clock::time_point& current_time);

	void execute_encounter_scripted_actions();
	void execute_neutral_scripted_actions();

private:
	void spawn_encounter(unsigned index_spawn);
	void send_notification_to_player(std::vector<npc_action> actions, pos central_position_actions, double distance_notif);

private:
	sol::state _lua;

	//! vector of encounter spawning points
	std::vector<spawning_point> _spawningPoints;


	//! vector of Spawned encounter, the index of the vector is the id of the spawning point corresponding
	std::vector<std::vector<npc_lua_instance>> _spawned_per_spawning_point;

	std::vector<std::vector<npc_lua_instance>> _neutral_npc_per_zone;

};

}

#endif //FYS_ONLINE_SCRIPT_ENGINE_HH
