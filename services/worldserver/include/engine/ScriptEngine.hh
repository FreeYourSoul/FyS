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


#ifndef FYS_ONLINE_SCRIPTENGINE_HH
#define FYS_ONLINE_SCRIPTENGINE_HH

#include <memory>
#include <chrono>
#include <sol/sol.hpp>
#include <engine/PlayersData.hh>
#include <CmlScriptDownloader.hh>

using namespace std::chrono_literals;

// forward declarations
namespace fys::ws {
class WorldServerContext;
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
struct SpawningPoint {

	//! Approximate Center of the spawning point, used in conjunction with distanceNotification
	Pos centerSpawningPoint = {0.0, 0.0};
	//! Distance from the centerSpawningPoint at which a player get notified of a monster move
	double distanceNotification;

	unsigned maxSpawned = 0;
	std::string idSpawningPoint;

	std::chrono::seconds spawningInterval = BASE_SPAWNING_INTERVAL;
	std::chrono::system_clock::time_point nextSpawn{};
};

/**
 * A representation of a NPC instance (NPC can be a neutral npc or an encounter)
 */
struct NPCLuaInstance {
	CharacterInfo info;

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
struct NPCAction {
	//! id defining a idle action
	static constexpr uint IDLE = 0u;
	//! id defining a move action
	static constexpr uint MOVE = 1u;
	//! id defining a stop move action
	static constexpr uint STOP = 2u;

	//! Id defining the npc in LUA engine
	uint npcLuaId = 0u;

	/**
	 * Id of the action made by the NPC
	 * 0 means no action are made (no notification should be sent)
	 * 1 means a move action is made (afterMove must be filled in this case)
	 * 2 means a move stopped (afterMove must be filled in this case)
	 * Another id is a specific action (animation..)
	 */
	uint actionId = IDLE;
	CharacterInfo afterMove {};
};

class ScriptEngine {
	//! CharacterInfo : position x, position y, velocity, angle
	using CharacterInfoLuaReturnType = std::tuple<double, double, double, double>;

public:
	ScriptEngine(const WorldServerContext& ctx);

	void executeEncounterScriptedActions();

	void spawnNewEncounters(const std::chrono::system_clock::time_point& currentTime);
	void registerNPCMovementScripts(const std::vector<std::string>& movScripts);
	void registerEncounterSpawnScript(const std::vector<std::string>& spawnScripts);
	void executeNeutralScriptedActions();

private:
	void registerCommon();
	void spawnEncounter(unsigned indexSpawn);
	void sendNotificationToPlayer(std::vector<NPCAction> actions, Pos centralPositionActions, double distanceNotif);

private:
	//! vector of encounter spawning points
	std::vector<SpawningPoint> _spawningPoints;


	//! vector of Spawned encounter, the index of the vector is the id of the spawning point corresponding
	std::vector<std::vector<NPCLuaInstance>> _spawnedPerSpawningPoint;

	std::vector<std::vector<NPCLuaInstance>> _neutralNpcPerZone;

	sol::state _lua;
};

}

#endif //FYS_ONLINE_SCRIPTENGINE_HH
