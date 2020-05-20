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

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#define SOL_SAFE_REFERENCES
#define SOL_SAFE_FUNCTION_CALLS
#define SOL_SAFE_FUNCTION
#include <sol/sol.hpp>

#include <WorldServerContext.hh>

#include <engine/ScriptEngine.hh>

namespace {

std::vector<fys::ws::Pos>
retrievePosVector(std::vector<std::pair<double, double>> vec)
{
	std::vector<fys::ws::Pos> ret;
	ret.reserve(vec.size());
	for (const auto&[x, y] : vec) {
		ret.push_back({x, y});
	}
	return ret;
}

}

namespace fys::ws {

ScriptEngine::ScriptEngine(const WorldServerContext& ctx)
		:_cache(std::filesystem::path(ctx.getPathToLocalStorage()),

		// Find and download script handler
		[](const std::string& keyScript, const std::string& pathDestination) {
		})
{

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

	auto characterInfo = _lua.new_usertype<CharacterInfo>("CharacterInfo");
	characterInfo["pos"] = &CharacterInfo::pos;
	characterInfo["velocity"] = &CharacterInfo::velocity;
	characterInfo["angle"] = &CharacterInfo::angle;

	auto npcMovements = _lua.new_usertype<NPCMovement>("NPCMovement");
	npcMovements["info"] = &NPCMovement::info;
	npcMovements["currentDestination"] = &NPCMovement::currentDestination;
	npcMovements["path"] = &NPCMovement::path;

	auto spawningEncounterArea = _lua.new_usertype<SpawningEncounterArea>("SpawningEncounterArea");
	spawningEncounterArea["nextSpawnCycle"] = &SpawningEncounterArea::nextSpawnCycle;
	spawningEncounterArea["spawningArea"] = &SpawningEncounterArea::spawningArea;
	spawningEncounterArea["maxSpawned"] = &SpawningEncounterArea::maxSpawned;
	spawningEncounterArea["displayKey"] = &SpawningEncounterArea::displayKey;

}

void
ScriptEngine::spawnNewEncounters(const std::chrono::system_clock::time_point& currentTime)
{
	for (unsigned i = 0; i < _spawningPoints.size(); ++i) {
		if (currentTime >= _spawningPoints.at(i).nextSpawn) {
			this->spawnEncounter(i);
			_spawningPoints.at(i).nextSpawn += _spawningPoints.at(i).spawningInterval;
		}
	}
}

void
ScriptEngine::spawnEncounter(unsigned indexSpawn)
{
	// Do not spawn encounter if the max is already reached
	if (_spawningPoints.at(indexSpawn).maxSpawned <= _spawnedPerSpawningPoint.at(indexSpawn).size()) {
		return;
	}

	const std::string makeEncounterNPCMovement = fmt::format("makeEncounterNPCMovement_{}", indexSpawn);
	sol::function luaMaker = _lua[makeEncounterNPCMovement];

	if (!luaMaker.valid()) {
		SPDLOG_ERROR("[LuaEngine] : function {} has not been created properly at init.", makeEncounterNPCMovement);
		return;
	}
	const Zone& spawningZone = _spawningPoints.at(indexSpawn).zone;
	LuaSpawningReturnType res = luaMaker(spawningZone.width, spawningZone.height, spawningZone.top, spawningZone.left);

	_spawnedPerSpawningPoint[indexSpawn].push_back(
			NPCMovement{
					CharacterInfo{
							Pos{std::get<0>(res), std::get<1>(res)},
							std::get<2>(res),
							std::get<3>(res)
					},
					::retrievePosVector(std::get<4>(res)),
					std::get<5>(res)
			});
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