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

ScriptEngine::ScriptEngine(const WorldServerContext&)
{
	registerCommon();
}

void
ScriptEngine::registerCommon()
{
	SPDLOG_INFO("Register LUA utilities");

	_lua.open_libraries(sol::lib::base, sol::lib::package);

	try {
		_lua.safe_script(R"(
			function getCharacterInfo(spawningPoint, luaId)
				if luaId >= spawningPoint.numbers then return nil end
			end

			function spawn(spawningPoint)
				for id, spawn in pairs(spawningPoint.spawned) do
					if spawn.isAlive == false then
						spawningPoint.spawned[id].isAlive = true
						return id
					end
				end
				return nil
			end

			function print_test()
				print("This is a print test")
			end

			function execMovement(spawningPoint)


				-- Increment the current step (maybe # can be used instead of numberSteps)
				spawningPoint.current = current + 1
				if spawningPoint.current == spawningPoint.numberSteps then
					spawningPoint.current = 0
				end
			end
			)"
		);
	}
	catch (const std::exception& e) {
		SPDLOG_ERROR("Error while registering basic content {} ", e.what());
	}

	auto position = _lua.new_usertype<Pos>("Pos");
	position["x"] = &Pos::x;
	position["y"] = &Pos::y;

	auto characterInfo = _lua.new_usertype<CharacterInfo>("CharacterInfo");
	characterInfo["pos"] = &CharacterInfo::pos;
	characterInfo["velocity"] = &CharacterInfo::velocity;
	characterInfo["angle"] = &CharacterInfo::angle;

	_lua["retrieveAngle"] = [](double x, double y, double destinationX, double destinationY) {
		return std::atan((y - destinationY) / (x - destinationX));
	};

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
	// Do not spawn encounter if the max monster has already been spawned!
	if (_spawningPoints.at(indexSpawn).maxSpawned <= _spawnedPerSpawningPoint.at(indexSpawn).size()) {
		return;
	}
	const std::string spNamespace = fmt::format("spawningPoint_{}", _spawningPoints.at(indexSpawn).idSpawningPoint);

	try {
		auto result = _lua["spawn"](_lua[spNamespace]);

		if (!result.valid()) {
			SPDLOG_ERROR("[lua] : function '{}' has not been created properly at init.", spNamespace);
			return;
		}
		CharacterInfoLuaReturnType res = _lua["getCharacterInfo"](_lua[spNamespace], static_cast<uint>(result));
		_spawnedPerSpawningPoint[indexSpawn].push_back(
				NPCLuaInstance{
						CharacterInfo{Pos{std::get<0>(res), std::get<1>(res)}, std::get<2>(res), std::get<3>(res)},
						static_cast<uint>(result)
				});
	}
	catch (const std::exception& e) {
		SPDLOG_ERROR("[lua] : An error occurred while spawning with spawning point '{}' : {}", spNamespace, e.what());
	}
}

void
ScriptEngine::executeScriptedActions()
{
	for (uint spawningPointId = 0; spawningPointId < _spawningPoints.size(); ++spawningPointId) {
		for (const auto& spawnedNPC : _spawnedPerSpawningPoint.at(spawningPointId)) {
			try {
				_lua.safe_script(fmt::format("{}:exec()", spawnedNPC.npcLuaVariableName));
			}
			catch (const std::exception& e) {
				SPDLOG_ERROR("[lua] : An error occurred while executing script action LuaVarName {} : \n[ERROR] : {}",
						spawnedNPC.npcLuaVariableName, e.what());
			}
		}
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