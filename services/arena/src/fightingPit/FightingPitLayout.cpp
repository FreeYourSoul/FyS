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
FightingPitLayout::getChangingSideTeamMembers()
{
	std::vector<std::shared_ptr<TeamMember>> result;

	for (auto&[userNameId, isMoving] : _movingFlagAlly) {
		if (isMoving) {
			auto&[userName, id] = userNameId;
			result.emplace_back(_partyTeams.get().getSpecificTeamMemberById(userName, id));
		}
		isMoving = false;
	}
	return result;
}

std::vector<std::shared_ptr<FightingContender>>
FightingPitLayout::getChangingSideContenders()
{
	std::vector<std::shared_ptr<FightingContender>> result;

	for (uint i = 0; i < _movingFlagContender.size(); ++i) {
		if (_movingFlagContender.at(i) == true) {
			result.emplace_back(_contenders.get().getFightingContender(i));
		}
		_movingFlagContender[i] = false;
	}
	return result;
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
FightingPitLayout::setAllyMoveInitiatePosition(TeamMember& tm, HexagonSide::Orientation side)
{
	if ((*tm._side).second != HexagonSide::Orientation::NONE) {
		SPDLOG_ERROR("Can't set initial position for ally {}.{} as position is not NONE", tm.getUserName(), tm.getName());
		return;
	}
	tm._side.move(side, true);
}

bool
FightingPitLayout::initiateContenderMove(std::shared_ptr<FightingContender> contender, HexagonSide::Orientation moveTo)
{
	if (contender->getHexagonSide().canMove(moveTo)) {
		contender->_moving = moveTo;
		_movingFlagContender[contender->getId()] = true;
		return true;
	}
	return false;
}

void
FightingPitLayout::initiateForceContenderMove(std::shared_ptr<FightingContender> contender, HexagonSide::Orientation moveTo)
{
	contender->_moving = moveTo;
	_movingFlagContender[contender->getId()] = true;
}

void
FightingPitLayout::initiateContenderMoveDir(std::shared_ptr<FightingContender> contender, data::MoveDirection moveDir)
{
	switch (moveDir) {
		case data::MoveDirection::BACK:
			initiateContenderMove(contender, contender->getHexagonSide().findBack());
			break;
		case data::MoveDirection::RIGHT:
			initiateContenderMove(contender, contender->getHexagonSide().findRight());
			break;
		case data::MoveDirection::LEFT:
			initiateContenderMove(contender, contender->getHexagonSide().findLeft());
			break;
	}
}

bool
FightingPitLayout::initiateMemberMove(std::shared_ptr<TeamMember> member, HexagonSide::Orientation moveTo)
{
	if (member->getHexagonSide().canMove(moveTo)) {
		member->_status.moving = moveTo;
		_movingFlagContender[member->getId()] = true;
		return true;
	}
	return false;
}

void
FightingPitLayout::initiateForceMemberMove(std::shared_ptr<TeamMember> member, HexagonSide::Orientation moveTo)
{
	member->_status.moving = moveTo;
	_movingFlagContender[member->getId()] = true;
}

void
FightingPitLayout::initiateMemberMoveDir(std::shared_ptr<TeamMember> member, data::MoveDirection moveDir)
{
	switch (moveDir) {
		case data::MoveDirection::BACK:
			initiateMemberMove(member, member->getHexagonSide().findBack());
			break;
		case data::MoveDirection::RIGHT:
			initiateMemberMove(member, member->getHexagonSide().findRight());
			break;
		case data::MoveDirection::LEFT:
			initiateMemberMove(member, member->getHexagonSide().findLeft());
			break;
	}
}

}