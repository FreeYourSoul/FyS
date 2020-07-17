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

#include <algorithm>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <world_server_context.hh>
#include <CmlKey.hh>
#include <fstream>

#include <engine/script_engine.hh>

namespace fys::ws {

namespace {

/**
 * Merge actions notification from a given spawning_point (central position, distance of notification),
 *
 * TO THINK ABOUT : Making a merge that decrease the amount of spawning points, The issue being that if
 * a spawning point get merged with another one, the distance of notification has to be enlarged. Otherwise
 * players may not see some encounters...
 *
 * @param report to merge into
 * @param actions reported on the given spawning point (to be merged)
 * @param central_position_actions central point of the spawning point in which the actions occurred
 * @param distance_notif distance notification at which players should be notified
 */
void
merge_spawning_point_in_report(npc_actions_report& report)
{

}

}

/**
 * A representation of a NPC instance (NPC can be a neutral npc or an encounter)
 */
struct npc_lua_instance {
	character_info info;

	//! Spawning Point namespace, empty in case of neutral encounter
	std::string sp_namespace;

	/**
	 * Id of the npc used to reference the NPC in LUA
	 * In case of encounter, this will represent the index of the NPC in the spawning point NPC list
	 * In case of neutral NPC, this represent the unique id of the NPC
	 */
	std::uint32_t npc_lua_id;
};

struct script_engine::internal {

	internal() = default;

	[[nodiscard]] npc_actions_report
	execute_encounter_scripted_actions()
	{
		npc_actions_report report;

		report.npc_actions.reserve(spawning_points.size());
		for (std::uint32_t sp_index = 0; sp_index < spawning_points.size(); ++sp_index) {

			const std::string_view id_spawning_point = spawning_points.at(sp_index).id_spawning_point;
			std::vector<npc_action> actions_executed;

			actions_executed.reserve(spawned_per_spawning_point.at(sp_index).size());
			for (std::uint32_t spawn_id = 0; spawn_id < spawned_per_spawning_point.at(sp_index).size(); ++spawn_id) {
				auto& npc = spawned_per_spawning_point.at(sp_index).at(spawn_id);
				try {
					unsigned action_id;
					double x, y, velocity, angle;

					sol::tie(action_id, x, y, velocity, angle) =
							lua["execAction"](lua[npc.sp_namespace], npc.npc_lua_id, std::ref(npc.info));

					if (action_id != npc_action::IDLE) {
						npc.info.position.x = x;
						npc.info.position.y = y;
						npc.info.velocity = velocity;
						npc.info.angle = angle;

						actions_executed.emplace_back(npc_action{npc.npc_lua_id, id_spawning_point, action_id, npc.info});
					}
				}
				catch (const std::exception& e) {
					SPDLOG_ERROR("[lua] : An error occurred while executing script action SpawningPoint {} npc {} : \n[ERROR] : {}",
							npc.sp_namespace, npc.npc_lua_id, e.what());
				}
			}

			report.npc_actions.emplace_back(std::move(actions_executed));
		}
//			merge_spawning_point_in_report(report);
		return report;
	}

	void
	spawn_encounter(unsigned index_spawn)
	{
		// Do not spawn encounter if the max monster has already been spawned!
		if (spawning_points.at(index_spawn).max_spawned <= spawned_per_spawning_point.at(index_spawn).size()) {
			return;
		}
		const std::string spNamespace = fmt::format("spawningPoint_{}", spawning_points.at(index_spawn).id_spawning_point);

		try {
			auto result = lua["spawn"](lua[spNamespace]);

			if (!result.valid()) {
				SPDLOG_ERROR("[lua] : function '{}' has not been created properly at init.", spNamespace);
				return;
			}
			character_info_ret_type res = lua["getCharacterInfo"](lua[spNamespace], static_cast<uint>(result));
			spawned_per_spawning_point[index_spawn].push_back(
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

	[[nodiscard]] npc_actions_report
	execute_neutral_scripted_actions()
	{
		npc_actions_report report;
		return report;
	}

	sol::state lua;

	//! vector of encounter spawning points
	std::vector<spawning_point> spawning_points;

	//! vector of Spawned encounter, the index of the vector is the id of the corresponding spawning point
	std::vector<std::vector<npc_lua_instance>> spawned_per_spawning_point;

	std::vector<std::vector<npc_lua_instance>> neutral_npc_per_zone;
};

script_engine::script_engine()
		:_intern(std::make_unique<internal>()) { }

script_engine::~script_engine() = default;

void
script_engine::spawn_new_encounters(const std::chrono::system_clock::time_point& current_time)
{
	for (unsigned i = 0; i < _intern->spawning_points.size(); ++i) {
		if (current_time >= _intern->spawning_points.at(i).next_spawn) {
			_intern->spawn_encounter(i);
			_intern->spawning_points.at(i).next_spawn += _intern->spawning_points.at(i).spawning_interval;
		}
	}
}

npc_actions_report
script_engine::execute_scripted_actions()
{
	npc_actions_report report = _intern->execute_encounter_scripted_actions();
	npc_actions_report report_neutral_script = _intern->execute_neutral_scripted_actions();

	// merge both reports

	report.notification_distances.reserve(report.notification_distances.size() + report_neutral_script.notification_distances.size());
	std::move(report_neutral_script.notification_distances.begin(), report_neutral_script.notification_distances.end(),
			std::back_inserter(report.notification_distances));

	report.central_positions.reserve(report.central_positions.size() + report_neutral_script.central_positions.size());
	std::move(report_neutral_script.central_positions.begin(), report_neutral_script.central_positions.end(),
			std::back_inserter(report.central_positions));

	report.npc_actions.reserve(report.npc_actions.size() + report_neutral_script.npc_actions.size());
	std::move(report_neutral_script.npc_actions.begin(), report_neutral_script.npc_actions.end(),
			std::back_inserter(report.npc_actions));

	return report;
}


////////////////////////////////////////////////////////////////////////////////////
/// ================== World Populator Script Engine Builder =================== ///
////////////////////////////////////////////////////////////////////////////////////

void
world_populator::generate_spawning_points(const std::string& spawning_point_config_path, const std::string& base_path)
{
	std::ifstream i(spawning_point_config_path);
	nlohmann::json jsonConfig;
	i >> jsonConfig;

	auto wsJson = jsonConfig["spawning_points"];
	unsigned index = 0;

	_script_engine->_intern->spawning_points.resize(wsJson.size());
	for (auto &[key, value] : wsJson.items()) {
		const std::string keyCml = value["key_zone"].get<std::string>();
		const std::string sp_namespace = get_sp_namespace_from_key(keyCml);

		_script_engine->_intern->spawning_points[index].spawning_interval = std::chrono::seconds(value["spawning_interval"].get<uint>());
		_script_engine->_intern->spawning_points[index].id_spawning_point = sp_namespace;
		_script_engine->_intern->spawning_points[index].display_key = value["display_key"].get<std::string>();

		try {
			_script_engine->_intern->lua.safe_script_file(get_path_from_key(base_path, keyCml));
			auto maxSpawn = _script_engine->_intern->lua[sp_namespace]["numbers"];
			auto visibilityDistance = _script_engine->_intern->lua[sp_namespace]["visibility_distance"];
			auto centerPos_y = _script_engine->_intern->lua[sp_namespace]["center_point"]["y"];
			auto centerPos_x = _script_engine->_intern->lua[sp_namespace]["center_point"]["x"];

			if (!visibilityDistance.valid()) {
				throw std::runtime_error("'visibility_distance' is not properly set'");
			}
			if (!maxSpawn.valid()) {
				throw std::runtime_error("'maxSpawn' is not properly set");
			}
			if (!centerPos_x.valid(), !centerPos_y.valid()) {
				throw std::runtime_error("'center_pos' is not properly set");
			}
			_script_engine->_intern->spawning_points[index].center_spawning_point = pos{
					static_cast<double>(centerPos_x),
					static_cast<double>(centerPos_y)
			};
			_script_engine->_intern->spawning_points[index].max_spawned = static_cast<uint>(maxSpawn);
			_script_engine->_intern->spawning_points[index].distance_notification = static_cast<uint>(visibilityDistance);

		}
		catch (const std::exception& e) {
			SPDLOG_ERROR("[INIT] An error occurred while instantiating SpawningPoints '{}' : {}",
					_script_engine->_intern->spawning_points[index].display_key, e.what());
		}
		++index;
	}
}

void
world_populator::register_common_lua_engine(const std::string& to_lua_init_file)
{
	_script_engine->_intern->lua.open_libraries(sol::lib::base, sol::lib::package);

	try {
		_script_engine->_intern->lua.safe_script_file(to_lua_init_file);
	}
	catch (const std::exception& e) {
		SPDLOG_ERROR("Error while initiating LUA engine : {} ", e.what());
	}

	auto position = _script_engine->_intern->lua.new_usertype<pos>("Pos");
	position["x"] = &pos::x;
	position["y"] = &pos::y;

	auto characterInfo = _script_engine->_intern->lua.new_usertype<character_info>("CharacterInfo");
	characterInfo["pos"] = &character_info::position;
	characterInfo["velocity"] = &character_info::velocity;
	characterInfo["angle"] = &character_info::angle;

	_script_engine->_intern->lua["retrieveAngle"] = [](double x, double y, double destinationX, double destinationY) {
		return std::atan((y - destinationY) / (x - destinationX));
	};
}

const std::vector<spawning_point>&
world_populator::get_spawning_point() const
{
	return _script_engine->_intern->spawning_points;
}

} // fys::ws