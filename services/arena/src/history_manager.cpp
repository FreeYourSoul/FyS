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
#include <fightingPit/fighting_pit.hh>
#include <nlohmann/json.hpp>
#include "history_manager.hh"

namespace {

[[nodiscard]] std::string
targetStr(std::optional<fys::arena::TargetType> target)
{
	return "";
}

[[nodiscard]] nlohmann::json
generate_json_target(fys::arena::history_action history_action)
{
	const std::optional<fys::arena::TargetType> &target = history_action.targets;
	if (target.has_value()) {

	}
	return {
			{"id", history_action.id_character},
			{"isContender", false}
	};
}

}

namespace fys::arena {

void
history_manager::add_historic(unsigned fp_id, history_action&& ha)
{
	if (get_instance()._is_manager_on) {
		auto it = get_instance()._history.find(fp_id);
		if (it != get_instance()._history.end()) {

			SPDLOG_INFO("[fp:{}] : Member {}.{} execute action '{}' targeting {}", fp_id,
					ha.id_character, ha.name, ha.action_key, targetStr(ha.targets));

			it->second.player_actions.emplace_back(std::move(ha));
		}
	}
}

void
history_manager::activate_historic_manager(bool on)
{
	get_instance()._is_manager_on = on;
}

void
history_manager::set_to_be_saved(std::uint32_t pit_id, bool toBeSaved)
{
	auto& instance = get_instance();
	if (instance._is_manager_on) {
		auto it = instance._history.find(pit_id);
		if (it != instance._history.end()) {
			it->second.has_to_be_saved = toBeSaved;
		}
	}
}

void
history_manager::create_historic(const fighting_pit& fp, unsigned seed)
{
	if (get_instance()._is_manager_on) {
		get_instance()._history[fp.id()].seed = seed;
	}
}

void
history_manager::save(std::uint32_t fp_id)
{
	auto& instance = get_instance();
	if (!instance._is_manager_on) return;

	auto history_it = instance._history.find(fp_id);
	if (history_it == instance._history.end()) return;

	if (history_it->second.has_to_be_saved && history_it->second.seed > 0) {
		SPDLOG_INFO("[fp:] HistoryAction : Save history file");
		auto date = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		nlohmann::json actions;

		for (const auto& pa : history_it->second.player_actions) {
			actions["actions"].push_back(nlohmann::json
					{
							{"id", pa.id_character},
							{"isContender", false},
							{"actionKey", pa.action_key},
							generate_json_target(pa)
					}
			);
		}

		nlohmann::json json = {
				{"date", std::ctime(&date)},
				actions
		};
	}
	get_instance()._history.erase(history_it);
}

}

