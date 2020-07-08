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


#ifndef FYS_FIGHTINGPITINSTANCE_HH
#define FYS_FIGHTINGPITINSTANCE_HH

#include <vector>
#include <optional>
#include <optional>
#include <chrono>

#include <chaiscript/chaiscript.hpp>

#include <fightingPit/contender/pit_contenders.hh>
#include <fightingPit/fighting_pit_layout.hh>
#include <fightingPit/side_battle.hh>
#include <fightingPit/rewards.hh>
#include <fightingPit/team/ally_party_teams.hh>
#include <chai_register.hh>
#include <fightingPit/team/team_member.hh>

// forward declarations
namespace zmq {
struct message_t;
}
namespace fys::arena {
class fighting_contender;
class worker_service;
class party_team;
}

namespace fys::arena {

// interval of time between 2 turn for a player depending on the difficulty
namespace interval {
constexpr static auto EASY = std::chrono::milliseconds(std::chrono::seconds(20));
constexpr static auto MEDIUM = std::chrono::milliseconds(std::chrono::seconds(15));
constexpr static auto HARD = std::chrono::milliseconds(std::chrono::seconds(10));
}

//! Actions struct containing all the information to forward to the member's pending action Queue
struct player_action {
	uint id_member;
	std::string action_name;
	std::vector<uint> contender_target;
	std::vector<uint> ally_target;
};

class fighting_pit_announcer;

/**
 * @brief A fighting pit represent a specific instance of a battle.
 *
 * A fightingpit is composed of:
 *  - a layout (mapping of how the monsters/players are spread on the board)
 *  - a PitContenders object managing all the monsters and their behaviours (scripting)
 *  - a AllyPartyTeams object managing the players, each client has a PartyTeam (multiple party teams possible for
 *    one AllyPartyTeams) and the PartyTeam is composed of TeamMember that are the actual characters spread on the
 *    gaming board.
 *  - The 18 instances of SideBattle
 *
 * @see SideBattle
 */
class fighting_pit {

	friend class fighting_pit_announcer;

	//! Progress Status Enum: represent the status of the battle
	enum class progress {
		//! joining fight status
		ON_HOLD,
		//! waiting creator readiness
		ON_HOLD_NOT_REACHABLE,

		//! battle is over, fighting pit has to be cleaned out
		CLEANUP,
		//! On going battle
		ON_GOING,
		//! Contender win, process the loss (if any) of the ally
		CONTENDER_WIN,
		//! Allies win, process the earning (if any) to the allies
		ALLY_WIN
	};

	//! Simple structure representing an authenticated player (pair name/token)
	struct authenticated_player {
		std::string name;
		std::string token;
	};

public:
	//! used as a wrong id when a fighting pit is wrongly generated
	static constexpr unsigned CREATION_ERROR = 0;

	//! Complexity of the FightingPit
	enum level : uint {
		EASY = 0,
		MEDIUM = 1,
		HARD = 2,
		NONE
	};

	explicit fighting_pit(std::string creator_user_name, level levelFightingPit);

	/**
	 *
	 * @param now
	 */
	void continue_battle(const std::chrono::system_clock::time_point& now);

	/**
	 *
	 * @param user
	 */
	void forward_to_team_member(const std::string& user, player_action action);

	/**
	 * Add an authenticated player in the fighting pit, the player authentication is not verified an thus must be
	 * verified beforehand.
	 * @param user_name unique name of the player to add
	 * @param user_token token of the player to add
	 */
	void add_authenticated_user(std::string user_name, std::string user_token);

	/**
	 * Used to add a new party team (incoming player in the fighting pit) and register its action to chai
	 *
	 * @note The verification if the fighting pit is in ON_HOLD mode is not done in this function and thus should be done before the
	 * PartyTeam object is created.
	 *
	 * @param pt party team to add
	 * @param cache retrieving object to find the actions
	 */
	void add_party_team_and_register_actions(std::unique_ptr<party_team> pt, cache::Cml& cache);

	/**
	 * Check if given player (defined by name/token) is authenticated
	 * @param name unique name of the player to check
	 * @param token token of the player to check
	 * @return true if the player is authenticated, false otherwise
	 */
	[[nodiscard]] bool
	is_player_participant(const std::string& name, const std::string& token) const;

	[[nodiscard]] const party_team&
	get_party_team_of_player(const std::string& user_name) const { return _party_teams.get_party_team_of_player(user_name); }

	[[nodiscard]] const std::unique_ptr<chaiscript::ChaiScript>&
	get_chai_ptr() const noexcept { return _chai_ptr; }

	[[nodiscard]] unsigned
	get_id() const noexcept { return _arena_id; }

	[[nodiscard]] bool
	is_joinable() const noexcept { return _progress == progress::ON_HOLD; }

	[[nodiscard]] const ally_party_teams&
	get_party_teams() const { return _party_teams; }

	[[nodiscard]] const pit_contenders&
	get_pit_contenders() const { return _contenders; }

	/**
	 * Check if the battle is done, the winner doesn't matter, this is a status to cleanup the fighting pit when its done
	 * @return true if the battle is over, false otherwise
	 */
	[[nodiscard]] bool
	is_battle_over() const noexcept { return _progress == progress::CLEANUP; }

	/**
	 * Check if the fight has started
	 *
	 * @return true if the fight is in on-going status, false otherwise
	 */
	[[nodiscard]] bool
	is_battle_on_going() const { return _progress == progress::ON_GOING; }


	/**
	 * Notify the clients if they won or lose, then set the fighting pit as ready for cleanup
	 * If a clients win, the rewards are broadcast to the players
	 * @param broadcast_handler handler to broadcast message to the players
	 */
	template <typename BroadcastHandler>
	void notify_end_status(BroadcastHandler&& broadcast_handler) {
		if (_progress == progress::ALLY_WIN) {
			std::forward<BroadcastHandler>(broadcast_handler)(make_winner_notification());
			_progress = progress::CLEANUP;
		}
		else if (_progress == progress::CONTENDER_WIN) {
			std::forward<BroadcastHandler>(broadcast_handler)(make_looser_notification());
			_progress = progress::CLEANUP;
		}
	}


	/**
	 * Disable the ability to join the battle if the fighting pit is currently reachable
	 */
	void disable_join() noexcept { if (is_joinable()) _progress = progress::ON_HOLD_NOT_REACHABLE; }
	void add_rewards(std::string action, uint quantity) noexcept
	{
		_rewards->keys.emplace_back(std::move(action));
		_rewards->quantity.emplace_back(quantity);
	};
	void set_player_readiness(const std::string& user_name);

	/**
	 * ArenaId (also called fightingPitId) is not set directly at construction time as it needs to be stored in the
	 * WorkerService to generate the id.
	 *
	 * @param arena_id id to set for the fightingPit
	 */
	void set_arena_id(unsigned arena_id) noexcept {
		_party_teams.set_fighting_pit_id(arena_id);
		_arena_id = arena_id;
	}

private:

	/**
	 * Check if the target is registered in the ChaiScript engine, if it is, check if the action requested
	 * by the player match the type of target required by the registered action.
	 * Finally check if the target exist in the fightingPit
	 * @param user Player name that is executing the action
	 * @param member name that is doing an action
	 * @param action information about the action to execute (contains target)
	 * @return a pair (success, target)
	 * @return_success : false if description doesn't match the requirement and/or target doesn't exist, true otherwise
	 * @return_target  : a TargetType (Variant) set with the proper type
	 */
	[[nodiscard]] std::pair<bool, std::optional<TargetType>>
	check_and_retrieve_target(const std::string& user, const team_member_sptr& member, const player_action& action);

	/**
	 * Verify if the fight has been won by a team (contender or ally) and return an appropriate Progress
	 * @return Progress::ALLY_WIN if the ally win, Progress::CONTENDER_WIN if the contender win, Progress::ON_GOING otherwise
	 */
	[[nodiscard]] progress
	update_progress_status();

	[[nodiscard]] bool
	add_contender(const std::shared_ptr<fighting_contender>& fc);

	[[nodiscard]] zmq::message_t
	make_winner_notification() const;

	[[nodiscard]] zmq::message_t
	make_looser_notification() const;

	//! Initialize the sideBattles vector in the same order as the enum in order to easily access a specific side
	void initialize_side_battles();
	void initialize_priority_in_side_battles();

private:
	progress _progress = progress::ON_HOLD;
	level _level_fighting_pit;
	std::chrono::milliseconds _time_interlude;
	pit_contenders _contenders;
	ally_party_teams _party_teams;

	// mapping of the contenders/NPC with the layout of the FightingPit
	fighting_pit_layout _layout_mapping;

	std::string _creator_user_name;
	unsigned _arena_id = 0;

	std::vector<authenticated_player> _authenticated_players;

	std::unique_ptr<chaiscript::ChaiScript> _chai_ptr;
	std::unique_ptr<rewards> _rewards;
	std::vector<side_battle> _side_battles;

};

}

#endif // !FYS_FIGHTINGPITINSTANCE_HH
