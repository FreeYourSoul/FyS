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

	std::ifstream i(arena["zone_configuration"].get<std::string>());
	json jsonConfig;
	i >> jsonConfig;
	auto& encounter = jsonConfig["encounter"];
	const std::string& zone = encounter["range"]["zone"].get<std::string>();
	_encounterContext._rangeEncounterPerZone[zone][0] = std::pair(encounter["range"]["easy"][0].get<uint>(), encounter["range"]["easy"][1].get<uint>());
	_encounterContext._rangeEncounterPerZone[zone][1] = std::pair(encounter["range"]["medium"][0].get<uint>(), encounter["range"]["medium"][1].get<uint>());
	_encounterContext._rangeEncounterPerZone[zone][2] = std::pair(encounter["range"]["hard"][0].get<uint>(), encounter["range"]["hard"][1].get<uint>());

	auto& contenders = encounter["contenders"];
	for (auto& contender : contenders) {
		EncounterContext::EncounterDesc desc = {
				contender["key"].get<std::string>(),
				contender.value("max_encountering", 99u),
				{
						contender["chance"]["easy"].get<uint>(),
						contender["chance"]["medium"].get<uint>(),
						contender["chance"]["hard"].get<uint>()
				},
				std::pair(contender["level_range"][0].get<uint>(), contender["level_range"][1].get<uint>())
		};
		_encounterContext._contendersPerZone[contender["zone"].get<std::string>()].emplace_back(std::move(desc));
	}
	if (!validateEncounterContext())
		throw std::runtime_error("Encounter Context invalid");
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
	str += "[INFO] Config file used: " + _configFile + "\n\n";
	str += "[INFO] Dispatcher connected port: " + std::to_string(_dispatcherData.port) + "\n";
	str += "[INFO] Dispatcher connected host: " + _dispatcherData.address + "\n";
	str += "[INFO] Dispatcher connection string: " + getDispatcherConnectionString() + "\n";
	str += "[INFO] Player binding string: " + getPlayerBindingString() + "\n";
	str += "\n*************************\n";
	return str;
}

std::string
ArenaServerContext::getDispatcherConnectionString() const noexcept
{
	return std::string("tcp://").append(_dispatcherData.address).append(":").append(std::to_string(_dispatcherData.port));
}

std::string
ArenaServerContext::getPlayerBindingString() const noexcept
{
	return std::string("tcp://*:").append(std::to_string(_playerConnectionPort));
}

}
