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

#include <fmt/ostream.h>
#include <spdlog/spdlog.h>
#include <utility>

#include <fightingPit/contender/PitContenders.hh>
#include <fightingPit/team/AllyPartyTeams.hh>
#include <fightingPit/FightingPitLayout.hh>
#include <fightingPit/team/TeamMember.hh>
#include <fightingPit/contender/ContenderScripting.hh>
#include <fightingPit/contender/FightingContender.hh>
#include <fightingPit/SideBattle.hh>

namespace fys::arena {

FightingPitLayout::FightingPitLayout(PitContenders& contenders, AllyPartyTeams& partyTeams)
		:_contenders(contenders), _partyTeams(partyTeams)
{
	for (auto& pt : partyTeams.getPartyTeams()) {
		addActivePartyTeam(*pt);
	}
	addActiveContender(contenders.getNumberContender());
}


void
FightingPitLayout::addActiveContender(uint numberContenderToAdd)
{
	_movingFlagContender.resize(_movingFlagContender.size() + numberContenderToAdd, false);
}

void
FightingPitLayout::addActivePartyTeam(const PartyTeam& pt)
{
	for (auto& tm : pt.getTeamMembers()) {
		_movingFlagAlly[std::pair(tm->getUserName(), tm->getId())] = false;
	}
}

unsigned
FightingPitLayout::activeCharactersOnSide(HexagonSide::Orientation side) const
{
	return _contenders.get().contenderOnSide(side) + _partyTeams.get().allyNumberOnSide(side);
}

void
FightingPitLayout::executeMovements(std::vector<SideBattle>& sides)
{
	auto changingSideContenders = getChangingSideContenders();
	auto changingSideAlly = getChangingSideTeamMembers();
	auto movementExecution = [&sides](auto& characterToMove) {
		HexagonSide::Orientation sideBeforeMove = (*characterToMove->_side).second;
		HexagonSide::Orientation sideAfterMove = characterToMove->_moving;
		if (characterToMove->_side.move(sideAfterMove, true)) {
			sides[static_cast<uint>(sideBeforeMove)].removeParticipantFromList(*characterToMove);
			sides[static_cast<uint>(sideAfterMove)].addParticipantInList(*characterToMove);
		}
	};

	std::for_each(changingSideAlly.begin(), changingSideAlly.end(), movementExecution);
	std::for_each(changingSideContenders.begin(), changingSideContenders.end(), movementExecution);
}

std::vector<std::shared_ptr<TeamMember>>
FightingPitLayout::getChangingSideTeamMembers() const
{
	std::vector<std::shared_ptr<TeamMember>> result;

	for (auto&[userNameId, isMoving] : _movingFlagAlly) {
		if (isMoving) {
			auto&[userName, id] = userNameId;
			result.emplace_back(_partyTeams.get().getSpecificTeamMemberById(userName, id));
		}
	}
	return result;
}

std::vector<std::shared_ptr<FightingContender>>
FightingPitLayout::getChangingSideContenders() const
{
	std::vector<std::shared_ptr<FightingContender>> result;

	for (uint i = 0; i < _movingFlagContender.size(); ++i) {
		if (_movingFlagContender.at(i)) {
			result.emplace_back(_contenders.get().getFightingContender(i));
		}
	}
	return result;
}

bool
FightingPitLayout::move(HexagonSide& side, data::MoveDirection directionToMove)
{
	if (directionToMove == data::MoveDirection::RIGHT) {
		if (!side.moveRight()) {
			SPDLOG_ERROR("Impossible move from {} to right", side);
			return false;
		}
	}
	else if (directionToMove == data::MoveDirection::LEFT) {
		if (!side.moveLeft()) {
			SPDLOG_ERROR("Impossible move from {} to left", side);
			return false;
		}
	}
	else if (directionToMove == data::MoveDirection::BACK) {
		if (!side.moveBack()) {
			SPDLOG_ERROR("Impossible move from {} to backside", side);
			return false;
		}
	}
	return true;
}

void
FightingPitLayout::setContenderInitiatePosition(FightingContender& contender, HexagonSide::Orientation side)
{
	if ((*contender._side).second != HexagonSide::Orientation::NONE) {
		SPDLOG_ERROR("Can't set initial position for contender {} as position is not NONE", contender.getName());
		return;
	}
	contender._side.move(side, true);
}

void
FightingPitLayout::setAllyMoveInitiatePosition(TeamMember& teamMember, HexagonSide::Orientation side)
{
	if ((*teamMember._side).second != HexagonSide::Orientation::NONE) {
		SPDLOG_ERROR("Can't set initial position for ally {}.{} as position is not NONE", teamMember.getUserName(), teamMember.getName());
		return;
	}
	teamMember._side.move(side, true);
}

}