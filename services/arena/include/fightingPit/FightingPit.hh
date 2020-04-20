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
#include <chrono>

#include <chaiscript/chaiscript.hpp>

#include <fightingPit/contender/PitContenders.hh>
#include <fightingPit/FightingPitLayout.hh>
#include <fightingPit/SideBattle.hh>
#include <fightingPit/team/AllyPartyTeams.hh>
#include <ChaiRegister.hh>

using namespace std::chrono_literals;

// forward declarations
namespace fys::arena {
class FightingContender;

class PartyTeam;
}

namespace fys::arena {

//! interval of time between 2 turn for a player depending on the difficulty
namespace interval {
constexpr static auto EASY = 20000ms;
constexpr static auto MEDIUM = 15000ms;
constexpr static auto HARD = 8000ms;
}

class FightingPitAnnouncer;

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
class FightingPit {

	friend class FightingPitAnnouncer;

	//! Progress Status Enum: represent the status of the battle
	enum class Progress {
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
	struct AuthenticatedPlayer {
		std::string name;
		std::string token;
	};

public:
	//! used as a wrong id when a fighting pit is wrongly generated
	static constexpr unsigned CREATION_ERROR = 0;

	//! Complexity of the FightingPit
	enum Level : uint {
		EASY = 0,
		MEDIUM = 1,
		HARD = 2,
		NONE
	};

	explicit FightingPit(std::string creatorUserName, Level levelFightingPit);

	/**
	 *
	 * @param now
	 */
	void continueBattle(const std::chrono::system_clock::time_point& now);

	/**
	 *
	 * @param userName
	 * @param idMember
	 * @param action string representing the action to do (has to be a registered action in chaiscript engine)
	 */
	void forwardMessageToTeamMember(const std::string& userName, unsigned int idMember, const std::string& action);

	/**
	 * Add an authenticated player in the fighting pit, the player authentication is not verified an thus must be
	 * verified beforehand.
	 * @param userName unique name of the player to add
	 * @param userToken token of the player to add
	 */
	void addAuthenticatedUser(std::string userName, std::string userToken);

	/**
	 * Used to add a new party team (incoming player in the fighting pit) and register its action to chai
	 *
	 * @note The verification if the fighting pit is in ON_HOLD mode is not done in this function and thus should be done before the
	 * PartyTeam object is created.
	 *
	 * @param pt party team to add
	 * @param cache retrieving object to find the actions
	 */
	void addPartyTeamAndRegisterActions(std::unique_ptr<PartyTeam> pt, cache::Cml& cache);

	/**
	 * Check if given player (defined by name/token) is authenticated
	 * @param name unique name of the player to check
	 * @param token token of the player to check
	 * @return true if the player is authenticated, false otherwise
	 */
	[[nodiscard]] bool
	isPlayerParticipant(const std::string& name, const std::string& token) const;

	[[nodiscard]] const PartyTeam&
	getPartyTeamOfPlayer(const std::string& userName) const { return _partyTeams.getPartyTeamOfPlayer(userName); }

	[[nodiscard]] const std::unique_ptr<chaiscript::ChaiScript>&
	getChaiPtr() const noexcept { return _chaiPtr; }

	[[nodiscard]] unsigned
	getId() const noexcept { return _arenaId; }

	[[nodiscard]] bool
	isJoinable() const noexcept { return _progress == Progress::ON_HOLD; }

	[[nodiscard]] const AllyPartyTeams&
	getPartyTeams() const { return _partyTeams; }

	[[nodiscard]] const PitContenders&
	getPitContenders() const { return _contenders; }

	/**
	 * Check if the battle is done, the winner doesn't matter, this is a status to cleanup the fighting pit when its done
	 * @return true if the battle is over, false otherwise
	 */
	[[nodiscard]] bool
	isBattleOver() const noexcept { return _progress == Progress::CLEANUP; }

	/**
	 * Disable the ability to join the battle if the fighting pit is currently reachable
	 */
	void disableJoin() noexcept { if (isJoinable()) _progress = Progress::ON_HOLD_NOT_REACHABLE; }
	void setPlayerReadiness(const std::string& userName);
	void setArenaId(unsigned arenaId) noexcept { _arenaId = arenaId; }

private:
	/**
	 * Check if the fight has started, if it has check if the fight is finished (winner determined)
	 * If ally wins, a generation of the loot is made and returned to the clients
	 * If enemy wins, a notification is sent to players
	 * @return true if the fight has been terminated, false otherwise
	 */
	[[nodiscard]] bool
	checkEndStatusFightingPit();

	/**
	 * Verify if the fight has been won by a team (contender or ally) and return an appropriate Progress
	 * @return Progress::ALLY_WIN if the ally win, Progress::CONTENDER_WIN if the contender win, Progress::ON_GOING otherwise
	 */
	[[nodiscard]] Progress
	updateProgressStatus();

	[[nodiscard]] bool
	addContender(const std::shared_ptr<FightingContender>& fc) { return _contenders.addContender(fc); }

	void initializeSideBattles();
	void initializePriorityListInSidesBattle();

private:
	Progress _progress = Progress::ON_HOLD;
	Level _levelFightingPit;
	std::chrono::milliseconds _timeInterlude;
	PitContenders _contenders;
	AllyPartyTeams _partyTeams;

	// mapping of the contenders/NPC with the layout of the FightingPit
	FightingPitLayout _layoutMapping;

	std::string _creatorUserName;
	unsigned _arenaId = 0;

	std::vector<AuthenticatedPlayer> _authenticatedPlayers;

	std::unique_ptr<chaiscript::ChaiScript> _chaiPtr;
	std::vector<SideBattle> _sideBattles;

};

}

#endif // !FYS_FIGHTINGPITINSTANCE_HH
