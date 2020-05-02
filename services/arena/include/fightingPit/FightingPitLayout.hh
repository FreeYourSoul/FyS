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


#ifndef FYS_FIGHTINGPITLAYOUT_HH
#define FYS_FIGHTINGPITLAYOUT_HH

#include <functional>
#include <map>

#include <fightingPit/HexagonSide.hh>

// forward declaration
namespace fys::arena {
class SideBattle;
class AllyPartyTeams;
class PitContenders;
class PartyTeam;
class TeamMember;
class FightingContender;
}
// end forward declaration


namespace fys::arena {

/**
 * @brief Layout of the fighting pit. Manage the positions of monsters and the players/monsters movements
 *
 * This class is mapping the characters (contenders or NPC) with the layout of the fightingpit.
 * The layout of the fighting pit can be taken as a draughtboard composed of 3 hexagons as shown as below
 *                        _____
 *                       /     \
 *                 _____/  B    \        18 possible playable sides
 *                /     \       /        6  possible ambush sides [A_NE, B_SW, A_SE, C_NW, C_N, B_S]
 *               /  A    \_____/
 *               \       /     \
 *                \_____/  C    \
 *                      \       /
 *                       \_____/
 *
 * Each hexagon represent the monster position. The Player can have character of its team spread on the lines representing
 * the sides of the hexagons.
 * Here is the hexagon at coordinate A, with its sides (where the characters can stands to attack the monster).
 *
 *           A_N
 *           _____                N  = North
 * A_NW     /     \    A_NE       NE = North-East
 *         /  A    \              NW = North-West
 *         \       /              S  = South
 * A_SW     \_____/    A_SE       SE = South-East
 *                                SW = South-West
 *           A_S
 *
 */
class FightingPitLayout {

public:
	FightingPitLayout(PitContenders& contenders, AllyPartyTeams& partyTeams);

	void addActivePartyTeam(const PartyTeam& pt);
	void addActiveContender(uint numberContenderToAdd = 1u);

	/**
	 * Retrieve the number of active characters (contenders or players) on the given side
	 * @param side to check the contender and/or players
	 * @return number of active player on the side
	 */
	[[nodiscard]] unsigned
	activeCharactersOnSide(HexagonSide::Orientation side) const;

	[[nodiscard]] bool
	initiateContenderMove(std::shared_ptr<FightingContender> contender, HexagonSide::Orientation moveTo);
	[[nodiscard]] bool
	initiateMemberMove(std::shared_ptr<TeamMember> member, HexagonSide::Orientation moveTo);

	void initiateForceContenderMove(std::shared_ptr<FightingContender> contender, HexagonSide::Orientation moveTo);
	void initiateForceMemberMove(std::shared_ptr<TeamMember> member, HexagonSide::Orientation moveTo);
	void initiateContenderMoveDir(std::shared_ptr<FightingContender> contender, data::MoveDirection moveDir);
	void initiateMemberMoveDir(std::shared_ptr<TeamMember> member, data::MoveDirection moveDir);

	void executeMovements(std::vector<SideBattle>& sides);

	static void setContenderInitiatePosition(FightingContender& contender, HexagonSide::Orientation side);
	static void setAllyMoveInitiatePosition(TeamMember& tm, HexagonSide::Orientation side);

private:
	[[nodiscard]] std::vector<std::shared_ptr<TeamMember>>
	getChangingSideTeamMembers();

	[[nodiscard]] std::vector<std::shared_ptr<FightingContender>>
	getChangingSideContenders();

private:
	std::reference_wrapper<PitContenders> _contenders;
	std::reference_wrapper<AllyPartyTeams> _partyTeams;

	//! map of <userName with memberId> on isMoving
	std::map<std::pair<std::string, uint>, bool> _movingFlagAlly;

	// vector of bool react differently from other vectors (memory optimization)
	// but it doesn't matter in this case as we are not in a multi-threaded context
	//! vector of isMoving for contenders (index of the vector is the id of the contender)
	std::vector<bool> _movingFlagContender;

};

}

#endif // !FYS_FIGHTINGPITLAYOUT_HH
