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

#include <spdlog/spdlog.h>
#include <sol/sol.hpp>

#include <WorldServerContext.hh>

#include <engine/ScriptEngine.hh>

namespace fys::ws {

ScriptEngine::ScriptEngine(const WorldServerContext& ctx)
		:_cache(std::filesystem::path(ctx.getPathToLocalStorage()),

		// Find and download script handler
		[](const std::string& keyScript, const std::string& pathDestination) {
		})
{

}

void
ScriptEngine::setScriptEngineCache(cache::CmlScriptDownloader cache)
{
	_cache = std::move(cache);
}

void
ScriptEngine::registerCommon()
{
	SPDLOG_INFO("Register LUA utilities");
	
	_lua.open_libraries(sol::lib::base, sol::lib::package);

	auto position = _lua.new_usertype<Pos>("Pos");
	position["x"] = &Pos::x;
	position["y"] = &Pos::y;

	auto zone = _lua.new_usertype<Zone>("Pos");
	zone["height"] = &Zone::height;
	zone["width"] = &Zone::width;
	zone["top"] = &Zone::top;
	zone["left"] = &Zone::left;

	auto npcMovements = _lua.new_usertype<NPCMovement>("NPCMovement");
	npcMovements["velocity"] = &NPCMovement::velocity;
	npcMovements["currentIndex"] = &NPCMovement::currentIndex;
	npcMovements["path"] = &NPCMovement::path;

	auto spawnedEncounter = _lua.new_usertype<SpawnedEncounter>("SpawnedEncounter");
	spawnedEncounter["position"] = &SpawnedEncounter::position;

	auto spawningEncounterArea = _lua.new_usertype<SpawningEncounterArea>("SpawningEncounterArea");
	spawningEncounterArea["nextSpawnCycle"] = &SpawningEncounterArea::nextSpawnCycle;
	spawningEncounterArea["spawningArea"] = &SpawningEncounterArea::spawningArea;
	spawningEncounterArea["maxSpawned"] = &SpawningEncounterArea::maxSpawned;
	spawningEncounterArea["displayKey"] = &SpawningEncounterArea::displayKey;

}

void
ScriptEngine::spawnNewEncounters()
{

}

void
ScriptEngine::registerNPCMovementScripts(const std::vector<std::string>& movScripts)
{

}

void
ScriptEngine::registerEncounterSpawnScript(const std::vector<std::string>& spawnScripts)
{

}

}