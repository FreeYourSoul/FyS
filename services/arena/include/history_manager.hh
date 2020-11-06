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

#ifndef FYS_ONLINE_HISTORY_MANAGER_HH
#define FYS_ONLINE_HISTORY_MANAGER_HH

#include <memory>
#include <unordered_map>
#include <vector>

#include <fightingPit/team/team_member.hh>

namespace fys::arena {
class fighting_pit;
}

namespace fys::arena {

/**
 * Store a specific action made during the arena
 * @note String are stored in string_views pointing to the fighting pit's data, as we have to ensure
 * the fighting pit instance is still alive until the end of the History process.
 */
struct history_action {
  //! in-game id of the player doing an action
  std::uint32_t id_character;
  //! name of the character
  std::string_view name;

  //! key of the action executed
  std::string_view action_key;
  //! in-game targets
  std::optional<target_type> targets;
};

/**
 * History manager, handle a single instance of the object via static method
 */
class history_manager {

  /**
   * History fight instance containing the list of actions that a player triggered during a fight.
   * The rest of the fight can be reproduced as the seed and the initial setup of the pit is provided.
   * This struct is filled via the static method provided by FightingHistoryManager.
   */
  struct history_fight {
	history_fight() = default;
	history_fight(unsigned seed)
		: seed(seed) {}

	//! Reference to the fighting pit
	//		std::reference_wrapper<const FightingPit> ref;
	//! Vector containing all the actions that occurred in the arena
	std::vector<history_action> player_actions{};

	//! Set at true in case an issue occurred in the fighting pit (will save the fighting pit in file format)
	bool has_to_be_saved = true;
	//! Seed on which the fight occurred
	unsigned seed = 0;// TODO make the random generator use a given seed, on instance of generator by pit will be needed
  };

public:
  static void activate_historic_manager(bool on);
  static void create_historic(const fighting_pit &fp, unsigned seed);
  static void add_historic(unsigned fp_id, history_action &&ha);
  static void set_to_be_saved(unsigned pit_id, bool toBeSaved);
  static void save(unsigned fp_id);

private:
  history_manager() = default;

  [[nodiscard]] static history_manager &
  get_instance() {
	static history_manager instance;
	return instance;
  }

private:
  std::unordered_map<unsigned, history_fight> _history;
  bool _is_manager_on = true;
};

}// namespace fys::arena

#endif//FYS_ONLINE_HISTORY_MANAGER_HH
