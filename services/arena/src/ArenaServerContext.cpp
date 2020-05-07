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
#include <numeric>
#include <utility>
#include <iostream>
#include <fstream>

#include "ArenaServerContext.hh"

using namespace nlohmann;

namespace fys::arena {

ArenaServerContext::ArenaServerContext(int ac, const char* const* av)
		:common::ServiceContextBase(ac, av)
{
	std::ifstream i(_configFile);
	SPDLOG_INFO("start parsing file {}", _configFile);
	json jsonConfig;
	i >> jsonConfig;
	parseArenaConfigFile(jsonConfig);
}

void
ArenaServerContext::parseArenaConfigFile(const json& configContent)
{

	auto& arena = configContent["Arena"];
	arena["code"].get_to(_code);
	arena["battle_threshold"].get_to(_battleThreshold);
	arena["player_connection_port"].get_to(_playerConnectionPort);
	arena["script_cache_source"].get_to(_pathSourceCache);
	arena["script_storage_cache"].get_to(_pathLocalStorageCache);
	arena["db_hostname"].get_to(_dbHost);
	arena["db_port"].get_to(_dbPort);

	auto zone_conf = arena["zone_configuration"];
	for (auto& zone : zone_conf) {
		SPDLOG_INFO("start parsing file {}", zone.get<std::string>());
		std::ifstream i(zone.get<std::string>());
		json jsonConfig;
		i >> jsonConfig;
		parseZoneConfigFile(jsonConfig);
	}
}

void
ArenaServerContext::parseZoneConfigFile(const nlohmann::json& configContent)
{
	auto& encounter_chance = configContent["encounter_chance"];
	auto& encounter_desc = configContent["encounter_desc"];
	const std::string& zone = configContent["zone"].get<std::string>();

	std::vector<std::string> keysEncounter;
	keysEncounter.reserve(encounter_desc.size());

	for (auto& encounterDesc : encounter_desc) {
		std::string keyEncounter = encounterDesc["key"].get<std::string>();
		EncounterContext::EncounterDesc desc = {
				encounterDesc["key"].get<std::string>(),
				encounterDesc.value("max_encountering", 99u),
				{
						encounter_chance[keyEncounter]["easy"].get<uint>(),
						encounter_chance[keyEncounter]["medium"].get<uint>(),
						encounter_chance[keyEncounter]["hard"].get<uint>()
				},
				std::pair(encounterDesc["level_range"][0].get<uint>(), encounterDesc["level_range"][1].get<uint>()),
		};

		auto& number_encounter = encounter_chance["number_encounter_range"];
		_encounterContext._rangeEncounterPerZone[zone] = {
				std::pair(number_encounter["easy"][0].get<uint>(), number_encounter["easy"][1].get<uint>()),
				std::pair(number_encounter["medium"][0].get<uint>(), number_encounter["medium"][1].get<uint>()),
				std::pair(number_encounter["hard"][0].get<uint>(), number_encounter["hard"][1].get<uint>())
		};

		_encounterContext._contendersPerZone[zone].emplace_back(std::move(desc));
		_encounterContext._rewardDescPerContender[keyEncounter] = getRewardDescriptionFromJson(encounterDesc["reward"]);
		keysEncounter.emplace_back(std::move(keyEncounter));
	}

	if (!validateEncounterContext())
		throw std::runtime_error("Encounter Context invalid");

	if (!validateRewardContext())
		throw std::runtime_error("Reward Context invalid");

}

EncounterContext::RewardEncounterDesc
ArenaServerContext::getRewardDescriptionFromJson(const nlohmann::json& rewardDesc) const
{
	EncounterContext::RewardEncounterDesc red;
	red.rangeDrop = {
			std::pair(rewardDesc["range"]["easy"][0].get<uint>(), rewardDesc["range"]["easy"][1].get<uint>()),
			std::pair(rewardDesc["range"]["medium"][0].get<uint>(), rewardDesc["range"]["medium"][1].get<uint>()),
			std::pair(rewardDesc["range"]["hard"][0].get<uint>(), rewardDesc["range"]["hard"][1].get<uint>())
	};
	auto& items = rewardDesc["items"];
	for (auto& item : items) {
		red.itemOnChanceRange[item["key"].get<std::string>()] = {
				item["chance"]["easy"].get<uint>(),
				item["chance"]["medium"].get<uint>(),
				item["chance"]["hard"].get<uint>()
		};
	}
	return red;
}

bool
ArenaServerContext::validateRewardContext() const
{
	for (const auto &[k, v] : _encounterContext._rewardDescPerContender) {
		for (int i = 0; i < 3; ++i) {
			uint total = 0;
			total = std::accumulate(v.itemOnChanceRange.cbegin(), v.itemOnChanceRange.cend(), 0,
					[i](const uint val, const auto& rhs) -> uint { return val + rhs.second[i]; });
			if (total != 100) {
				SPDLOG_ERROR("Reward Context invalid because of % chance for contender {} : "
							 "difficulty {} is {} while it should be equal 100", k, i, total);
				return false;
			}
		}
	}
	return true;
}

bool
ArenaServerContext::validateEncounterContext() const
{
	for (const auto &[k, v] : _encounterContext._rangeEncounterPerZone) {
		for (int i = 0; i < 3; ++i) {
			if (v.at(i).first <= 0) {
				SPDLOG_ERROR("Encounter Context invalid because of range for {} : "
							 "difficulty {} minimum is 0 or less while it should be at least 1", k, i);
				return false;
			}
		}
	}
	for (const auto &[k, v] : _encounterContext._contendersPerZone) {
		for (int i = 0; i < 3; ++i) {
			uint total = 0;
			total = std::accumulate(v.cbegin(), v.cend(), 0,
					[i](const uint val, const auto& rhs) -> uint { return val + rhs.chance[i]; });
			if (total != 100) {
				SPDLOG_ERROR("Encounter Context invalid because of % chance for zone {} : "
							 "difficulty {} is {} while it should be equal 100", k, i, total);
				return false;
			}
		}
	}
	return true;
}

std::string
ArenaServerContext::toString() const noexcept
{
	std::string str;
	str = "dump context\n*************************\n";
	str += "[INFO] Service " + _name + " context VERSION: " + _version + "\n";
	str += "[INFO] Config file used: " + _configFile + "\n";
	str += "[INFO] Handle zones: ";
	for (auto&[k, v] : _encounterContext._rangeEncounterPerZone) str += k + " ";
	str += "\n[INFO] Dispatcher connected port: " + std::to_string(_dispatcherData.port) + "\n";
	str += "[INFO] Dispatcher connected host: " + _dispatcherData.address + "\n";
	str += "[INFO] Dispatcher connection string: " + getDispatcherConnectionString() + "\n";
	str += "[INFO] Player binding string: " + getPlayerBindingString() + "\n";
	str += "[INFO] Local cache folder: " + getPathLocalStorageCache() + "\n";
	str += "\n*************************\n";
	return str;
}

std::string
ArenaServerContext::getPlayerBindingString() const noexcept
{
	return std::string("tcp://*:").append(std::to_string(_playerConnectionPort));
}

}
