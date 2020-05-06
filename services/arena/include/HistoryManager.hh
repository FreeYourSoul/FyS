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

#ifndef FYS_ONLINE_HISTORYMANAGER_HH
#define FYS_ONLINE_HISTORYMANAGER_HH

#include <unordered_map>
#include <memory>
#include <vector>

#include <fightingPit/team/TeamMember.hh>

namespace fys::arena {
class FightingPit;
}

namespace fys::arena {

/**
 * Store a specific action made during the arena
 * @note String are stored in string_views pointing to the fighting pit's data, as we have to ensure
 * the fighting pit instance is still alive until the end of the History process.
 */
struct HistoryAction {
	//! in-game id of the player doing an action
	uint idCharacter;
	//! name of the character
	std::string_view name;

	//! key of the action executed
	std::string_view actionKey;
	//! in-game targets
	std::optional<TargetType> targets;
};

/**
 * History manager, handle a single instance of the object via static method
 */
class HistoryManager {

	/**
	 * History fight instance containing the list of actions that a player triggered during a fight.
	 * The rest of the fight can be reproduced as the seed and the initial setup of the pit is provided.
	 * This struct is filled via the static method provided by FightingHistoryManager.
	 */
	struct HistoryFight {
		HistoryFight() = default;
		HistoryFight(unsigned seed)
				:seed(seed) { }

		//! Reference to the fighting pit
//		std::reference_wrapper<const FightingPit> ref;
		//! Vector containing all the actions that occurred in the arena
		std::vector<HistoryAction> playerActions{};

		//! Set at true in case an issue occurred in the fighting pit (will save the fighting pit in file format)
		bool hasToBeSaved = true;
		//! Seed on which the fight occurred
		unsigned seed = 0; // TODO make the random generator use a given seed, on instance of generator by pit will be needed
	};

public:
	static void setHistoricManagerOn(bool on);
	static void createHistoric(const FightingPit& fp, unsigned seed);
	static void addHistoric(unsigned fightingPitId, HistoryAction&& ha);
	static void setToBeSaved(unsigned fightingPitId, bool toBeSaved);
	static void save(unsigned fightingPitId);

private:
	HistoryManager() = default;

	[[nodiscard]] static HistoryManager&
	getInstance()
	{
		static HistoryManager instance;
		return instance;
	}

private:
	std::map<unsigned, HistoryFight> _history;
	bool _isManagerOn = true;

};

}

#endif //FYS_ONLINE_HISTORYMANAGER_HH
