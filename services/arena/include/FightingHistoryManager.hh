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

#ifndef FYS_ONLINE_FIGHTINGHISTORYMANAGER_HH
#define FYS_ONLINE_FIGHTINGHISTORYMANAGER_HH

#include <unordered_map>
#include <memory>
#include <vector>

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
	//! is the character an ally or a contender
	bool isContender;
	//! name of the character
	std::string_view name;

	//! key of the action executed
	std::string_view actionKey;
	//! in-game id of the contender targets
	std::vector<uint> idContenderTarget;
	//! in-game id of the allies targets
	std::vector<uint> idAllyTarget;
};

/**
 * History manager, handle a single instance of the object via static method
 */
class FightingHistoryManager {

	/**
	 * History fight instance containing the list of actions that occurred during a fight
	 * This struct is filled via the static method provided by FightingHistoryManager.
	 * if the seed is set (superior to 0) and if the hasToBeSaved variable has been set to true
	 */
	struct HistoryFight {
		HistoryFight(const FightingPit& pt, unsigned seed) noexcept
				:ref(pt), seed(seed) { }

		//! Reference to the fighting pit
		std::reference_wrapper<const FightingPit> ref;
		//! Vector containing all the actions that occurred in the arena
		std::vector<HistoryAction> hist;

		//! Set at true in case an issue occurred in the fighting pit (will save the fighting pit in file format)
		bool hasToBeSaved = false;
		//! Seed on which the fight occurred
		unsigned seed = 0;
	};

	inline static std::unique_ptr<FightingHistoryManager> _instance = nullptr;

public:
	static void setHistoricManagerOn(bool on);
	static void createHistoric(const FightingPit& fp, unsigned seed);
	static void addHistoric(unsigned fightingPitId, HistoryAction&& ha);
	static void setToBeSaved(unsigned fightingPitId, bool toBeSaved);
	static void save(unsigned fightingPitId);

private:
	explicit FightingHistoryManager() noexcept = default;

	[[nodiscard]] static FightingHistoryManager&
	getInstance()
	{
		if (!_instance) {
			_instance = std::unique_ptr<FightingHistoryManager>(new FightingHistoryManager());
		}
		return *_instance;
	}

private:
	std::unordered_map<unsigned, HistoryFight> _history;
	bool _isManagerOn = true;

};

}

#endif //FYS_ONLINE_FIGHTINGHISTORYMANAGER_HH
