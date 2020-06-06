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

[[nodiscard]] bool
isActionToBeNotified(uint actionId)
{
	return actionId > fys::ws::NPCAction::IDLE;
}

[[nodiscard]] bool
isActionMovementRelated(uint actionId)
{
	return actionId == fys::ws::NPCAction::MOVE || actionId == fys::ws::NPCAction::STOP;
}

}

namespace fys::ws {

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
						spNamespace,
						static_cast<uint>(result)
				});
	}
	catch (const std::exception& e) {
		SPDLOG_ERROR("[lua] : An error occurred while spawning with spawning point '{}' : {}", spNamespace, e.what());
	}
}

void
ScriptEngine::executeEncounterScriptedActions()
{
	for (uint spawningPointId = 0; spawningPointId < _spawningPoints.size(); ++spawningPointId) {
		std::vector<NPCAction> actionsExecuted;

		actionsExecuted.resize(_spawnedPerSpawningPoint.at(spawningPointId).size());
		for (uint spawnId = 0; spawnId < _spawnedPerSpawningPoint.at(spawningPointId).size(); ++spawnId) {
			const auto& npc = _spawnedPerSpawningPoint.at(spawningPointId).at(spawnId);
			try {
				uint luaId, actionId;
				double x, y, velocity, angle;
				sol::tie(luaId, actionId, x, y, velocity, angle) =
						_lua["execMovement"](_lua[npc.spNamespace], npc.npcLuaId, std::ref(npc.info));

				actionsExecuted.emplace_back(NPCAction{luaId, actionId, CharacterInfo{{x, y}, velocity, angle}});
			}
			catch (const std::exception& e) {
				SPDLOG_ERROR("[lua] : An error occurred while executing script action SpawningPoint {} npc {} : \n[ERROR] : {}",
						npc.spNamespace, npc.npcLuaId, e.what());
			}
		}

		sendNotificationToPlayer(std::move(actionsExecuted),
				_spawningPoints.at(spawningPointId).centerSpawningPoint,
				_spawningPoints.at(spawningPointId).distanceNotification);
	}
}

void
ScriptEngine::executeNeutralScriptedActions()
{

}

void
ScriptEngine::sendNotificationToPlayer(std::vector<NPCAction> actions, Pos centralPositionActions, double distanceNotif)
{
	// todo send the notifications
	// Maybe concentrate all merge all notification related to a specific spawning point and send this
	// full list to close player of the spawning point. It would save the processing time for each encounter
	// of who are the closest player to send the message to.
	// Downside: Maybe too big of a message... But could be split into multiple one if too big ?
}

}