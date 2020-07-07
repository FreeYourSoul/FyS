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
generateJsonTarget(fys::arena::history_action historyAction)
{
	const std::optional<fys::arena::TargetType> &target = historyAction.targets;
	if (target.has_value()) {

	}
	return {
			{"id", historyAction.id_character},
			{"isContender", false}
	};
}

}

namespace fys::arena {

void
history_manager::addHistoric(unsigned fightingPitId, history_action&& ha)
{
	if (getInstance()._is_manager_on) {
		auto it = getInstance()._history.find(fightingPitId);
		if (it != getInstance()._history.end()) {

			SPDLOG_INFO("[fp:{}] : Member {}.{} execute action '{}' targeting {}", fightingPitId,
					ha.id_character, ha.name, ha.action_key, targetStr(ha.targets));

			it->second.player_actions.emplace_back(std::move(ha));
		}
	}
}

void
history_manager::setHistoricManagerOn(bool on)
{
	getInstance()._is_manager_on = on;
}

void
history_manager::setToBeSaved(unsigned int fightingPitId, bool toBeSaved)
{
	auto& instance = getInstance();
	if (instance._is_manager_on) {
		auto it = instance._history.find(fightingPitId);
		if (it != instance._history.end()) {
			it->second.has_to_be_saved = toBeSaved;
		}
	}
}

void
history_manager::createHistoric(const fighting_pit& fp, unsigned seed)
{
	if (getInstance()._is_manager_on) {
		getInstance()._history[fp.get_id()].seed = seed;
	}
}

void
history_manager::save(unsigned int fightingPitId)
{
	auto& instance = getInstance();
	if (!instance._is_manager_on) return;

	auto historyIt = instance._history.find(fightingPitId);
	if (historyIt == instance._history.end()) return;

	if (historyIt->second.has_to_be_saved && historyIt->second.seed > 0) {
		SPDLOG_INFO("[fp:] HistoryAction : Save history file");
		auto date = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		nlohmann::json actions;

		for (const auto& playerAction : historyIt->second.player_actions) {
			actions["actions"].push_back(nlohmann::json
					{
							{"id", playerAction.id_character},
							{"isContender", false},
							{"actionKey", playerAction.action_key},
							generateJsonTarget(playerAction)
					}
			);
		}

		nlohmann::json json = {
				{"date", std::ctime(&date)},
				actions
		};
	}
	getInstance()._history.erase(historyIt);
}

}

