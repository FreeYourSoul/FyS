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


#ifndef FYS_ARENASERVERCONTEXT_HH
#define FYS_ARENASERVERCONTEXT_HH

#include <array>
#include <nlohmann/json.hpp>
#include <service_context_base.hh>

namespace fys::arena {

struct EncounterContext {
	using RngRange = std::pair<uint, uint>;
	using ChanceArray = std::array<uint, 3>;

	struct EncounterDesc {
		[[nodiscard]] bool
		operator!=(const EncounterDesc& other) const
		{
			return std::make_tuple(key, maxEncountering, chance, levelRange) !=
					std::make_tuple(other.key, other.maxEncountering, other.chance, other.levelRange);
		}

		//! key of the encounter to find it back via the cache
		std::string key;
		//! Max number of this type of monster you can encounter at once
		uint maxEncountering;
		//! percentage of chance to encounter this monster out of the 3 different difficulties
		ChanceArray chance;
		//! Range of level the monster is when encountered
		RngRange levelRange;

	};

	struct RewardEncounterDesc {
		//! range of items than can be dropped for an encounter out of the 3 different difficulties
		std::array<RngRange, 3> rangeDrop;
		//! map of items key over the chance of drop out of the 3 different difficulties
		std::map<std::string, ChanceArray> itemOnChanceRange;
	};

	/**
	 * Verify if the given zone is registered in the context
	 * @param wsId zone to check
	 * @return true if the zone is registered, false otherwise
	 */
	[[nodiscard]] bool
	zoneRegistered(const std::string& wsId) const noexcept { return _contendersPerZone.find(wsId) != _contendersPerZone.cend(); }

	//! range of number of monster findable per zone
	std::map<std::string, std::array<RngRange, 3>> _rangeEncounterPerZone;
	//! contender findable per zone
	std::map<std::string, std::vector<EncounterDesc>> _contendersPerZone;
	//! reward description per contender
	std::map<std::string, RewardEncounterDesc> _rewardDescPerContender;
};

class ArenaServerContext : public fys::common::service_context_base {

	//! Minimum of number of battle in parralele that can be processed by the Arena Server
	static constexpr unsigned MINIMUM_BATTLE_THRESHOLD = 3;

public:
	ArenaServerContext(int ac, const char* const* av);

	[[nodiscard]] std::string
	toString() const noexcept;

	[[nodiscard]] std::string
	getPlayerBindingString() const noexcept;

	[[nodiscard]] const std::string&
	getServerCode() const noexcept { return _code; }

	[[nodiscard]] const EncounterContext&
	getEncounterContext() const noexcept { return _encounterContext; }

	[[nodiscard]] const std::string&
	getPathLocalStorageCache() const noexcept { return _pathLocalStorageCache; }

	[[nodiscard]] const std::string&
	getPathSourceCache() const noexcept { return _pathSourceCache; }

	[[nodiscard]] const std::string&
	getDbHost() const noexcept { return _dbHost; }

	[[nodiscard]] uint
	getDbPort() const noexcept { return _dbPort; }

	[[nodiscard]] uint
	getBattleThreshold() const noexcept { return _battleThreshold; }

private:
	[[nodiscard]] bool
	validateEncounterContext() const;

	[[nodiscard]] bool
	validateRewardContext() const;

	[[nodiscard]] EncounterContext::RewardEncounterDesc
	getRewardDescriptionFromJson(const nlohmann::json& rewardDesc) const;

	void parseArenaConfigFile(const nlohmann::json& configContent);
	void parseZoneConfigFile(const nlohmann::json& configContent);

private:

	//! Code of the current ArenaServerService
	std::string _code;

	//! Path representing the local temporary storage created and maintained by Cml
	std::string _pathLocalStorageCache;
	//! Path of the reference data for CmlCopy
	std::string _pathSourceCache;

	//! Database hostname
	std::string _dbHost;
	//! Database port number
	uint _dbPort = 3306;

	//! Database hostname
	uint _playerConnectionPort;

	//! Maximum number of concurrent battle handled by the arena service
	uint _battleThreshold = MINIMUM_BATTLE_THRESHOLD;

	//! Context part specific to encounter handled by this arena server
	EncounterContext _encounterContext;
};

}

#endif // !FYS_ARENASERVERCONTEXT_HH
