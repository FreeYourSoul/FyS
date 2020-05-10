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
#include <sol/sol.hpp>
#include <engine/PlayersData.hh>
#include <CmlScriptDownloader.hh>

// forward declarations
namespace fys::ws {
class WorldServerContext;
}
// end forward declarations

namespace fys::ws {

struct SpawnedEncounter {
	Pos position;
};

struct SpawningEncounterArea {

	Zone spawningArea;
	std::string displayKey;

	unsigned maxSpawned;
	unsigned spawnCycleInterval;
	unsigned nextSpawnCycle;
};

struct NPCMovement {
	double velocity;
	uint currentIndex;
	std::vector<Pos> path;
};

class ScriptEngine {

public:
	ScriptEngine(const WorldServerContext& ctx);
	void setScriptEngineCache(cache::CmlScriptDownloader cache);

	void spawnNewEncounters();

	void registerCommon();
	void registerNPCMovementScripts(const std::vector<std::string>& movScripts);
	void registerEncounterSpawnScript(const std::vector<std::string>& spawnScripts);

private:
	cache::CmlScriptDownloader _cache;

	std::vector<NPCMovement> _npcMovements;
	std::vector<SpawnedEncounter> _spawningPoints;

	//! vector of Spawned encounter, the index of the vector is the id of the spawning point corresponding
	std::vector<std::vector<SpawnedEncounter>> _spawnedPerSpawningPoint;

	sol::state _lua;

};

}

#endif //FYS_ONLINE_SCRIPTENGINE_HH
