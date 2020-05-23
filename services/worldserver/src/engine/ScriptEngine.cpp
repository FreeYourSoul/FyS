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

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <WorldServerContext.hh>
#include <CmlKey.hh>

#include <engine/ScriptEngine.hh>

namespace {

void
testingScriptDownload(const std::string& key, const std::string& pathDest)
{
	fys::cache::CmlKey k("", key);
	auto p = std::filesystem::path(key);
	std::error_code e;

	std::filesystem::create_directories(p.parent_path(), e);
	std::filesystem::copy(k.getPath(), p, e);
}

}

namespace fys::ws {

ScriptEngine::ScriptEngine(const WorldServerContext& ctx)
		:_cache(std::filesystem::path(ctx.getPathToLocalStorage()),

		// Find and download script handler
		[](const std::string& keyScript, const std::string& pathDestination) {
			::testingScriptDownload(keyScript, pathDestination);
		})
{
	registerCommon();
}

void
ScriptEngine::registerCommon()
{
	SPDLOG_INFO("Register LUA utilities");

	_lua.open_libraries(sol::lib::base, sol::lib::package);

	auto position = _lua.new_usertype<Pos>("Pos");
	position["x"] = &Pos::x;
	position["y"] = &Pos::y;

	auto npcAction = _lua.new_usertype<NPCAction>("NPCAction");
	npcAction["destination"] = &NPCAction::destination;
	npcAction["idleTime"] = &NPCAction::idleTime;

	auto characterInfo = _lua.new_usertype<CharacterInfo>("CharacterInfo");
	characterInfo["pos"] = &CharacterInfo::pos;
	characterInfo["velocity"] = &CharacterInfo::velocity;
	characterInfo["angle"] = &CharacterInfo::angle;

	auto npcMovements = _lua.new_usertype<NPCInstance>("NPCInstance");
	npcMovements["info"] = &NPCInstance::info;
	npcMovements["currentAction"] = &NPCInstance::currentAction;
	npcMovements["actions"] = &NPCInstance::actions;

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

	const std::string makeEncounterNPCMovement = fmt::format("makeEncounterNPCAction_{}", indexSpawn);
	try {
		sol::function luaEncounterBuilder = _lua[makeEncounterNPCMovement];

		if (!luaEncounterBuilder.valid()) {
			SPDLOG_ERROR("[lua] : function '{}' has not been created properly at init.", makeEncounterNPCMovement);
			return;
		}
		NPCInstance res = luaEncounterBuilder();
		_spawnedPerSpawningPoint[indexSpawn].push_back(std::move(res));
	}
	catch (const std::exception& e) {
		SPDLOG_ERROR("[lua] : An error occurred while spawning a monster '{}' : {}", makeEncounterNPCMovement, e.what());
	}
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