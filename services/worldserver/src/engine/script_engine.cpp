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

#include <world_server_context.hh>
#include <CmlKey.hh>

#include <engine/script_engine.hh>

namespace {

//void
//testingScriptDownload(const std::string& key, const std::string& pathDest)
//{
//	fys::cache::CmlKey k("", key);
//	auto p = std::filesystem::path(key);
//	std::error_code e;
//
//	std::filesystem::create_directories(p.parent_path(), e);
//	std::filesystem::copy(k.getPath(), p, e);
//}

//[[nodiscard]] bool
//isActionToBeNotified(uint actionId)
//{
//	return actionId > fys::ws::NPCAction::IDLE;
//}
//
//[[nodiscard]] bool
//isActionMovementRelated(uint actionId)
//{
//	return actionId == fys::ws::NPCAction::MOVE || actionId == fys::ws::NPCAction::STOP;
//}

}

namespace fys::ws {

void
script_engine::spawn_new_encounters(const std::chrono::system_clock::time_point& current_time)
{
	for (unsigned i = 0; i < _spawningPoints.size(); ++i) {
		if (current_time >= _spawningPoints.at(i).next_spawn) {
			this->spawn_encounter(i);
			_spawningPoints.at(i).next_spawn += _spawningPoints.at(i).spawning_interval;
		}
	}
}

void
script_engine::spawn_encounter(unsigned index_spawn)
{
	// Do not spawn encounter if the max monster has already been spawned!
	if (_spawningPoints.at(index_spawn).max_spawned <= _spawned_per_spawning_point.at(index_spawn).size()) {
		return;
	}
	const std::string spNamespace = fmt::format("spawningPoint_{}", _spawningPoints.at(index_spawn).id_spawning_point);

	try {
		auto result = _lua["spawn"](_lua[spNamespace]);

		if (!result.valid()) {
			SPDLOG_ERROR("[lua] : function '{}' has not been created properly at init.", spNamespace);
			return;
		}
		character_info_ret_type res = _lua["getCharacterInfo"](_lua[spNamespace], static_cast<uint>(result));
		_spawned_per_spawning_point[index_spawn].push_back(
				npc_lua_instance{
						character_info{pos{std::get<0>(res), std::get<1>(res)}, std::get<2>(res), std::get<3>(res)},
						spNamespace,
						static_cast<uint>(result)
				});
	}
	catch (const std::exception& e) {
		SPDLOG_ERROR("[lua] : An error occurred while spawning with spawning point '{}' : {}", spNamespace, e.what());
	}
}

void
script_engine::execute_encounter_scripted_actions()
{
	for (uint spawningPointId = 0; spawningPointId < _spawningPoints.size(); ++spawningPointId) {
		std::vector<npc_action> actionsExecuted;

		actionsExecuted.resize(_spawned_per_spawning_point.at(spawningPointId).size());
		for (uint spawnId = 0; spawnId < _spawned_per_spawning_point.at(spawningPointId).size(); ++spawnId) {
			auto& npc = _spawned_per_spawning_point.at(spawningPointId).at(spawnId);
			try {
				unsigned actionId;
				double x, y, velocity, angle;

				sol::tie(actionId, x, y, velocity, angle) =
						_lua["execAction"](_lua[npc.spNamespace], npc.npcLuaId, std::ref(npc.info));

				npc.info.position.x = x;
				npc.info.position.y = y;
				npc.info.velocity = velocity;
				npc.info.angle = angle;
				actionsExecuted.emplace_back(npc_action{npc.npcLuaId, actionId, npc.info});
			}
			catch (const std::exception& e) {
				SPDLOG_ERROR("[lua] : An error occurred while executing script action SpawningPoint {} npc {} : \n[ERROR] : {}",
						npc.spNamespace, npc.npcLuaId, e.what());
			}
		}

		send_notification_to_player(std::move(actionsExecuted),
				_spawningPoints.at(spawningPointId).center_spawning_point,
				_spawningPoints.at(spawningPointId).distance_notification);
	}
}

void
script_engine::execute_neutral_scripted_actions()
{

}

void
script_engine::send_notification_to_player(std::vector<npc_action> actions, pos central_position_actions, double distance_notif)
{
	// todo send the notifications
	// Maybe concentrate all merge all notification related to a specific spawning point and send this
	// full list to close player of the spawning point. It would save the processing time for each encounter
	// of who are the closest player to send the message to.
	// Downside: Maybe too big of a message... But could be split into multiple one if too big ?
}

}