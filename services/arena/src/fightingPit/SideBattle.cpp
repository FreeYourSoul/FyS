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

#include <fightingPit/SideBattle.hh>
#include <fightingPit/contender/PitContenders.hh>
#include <fightingPit/contender/FightingContender.hh>
#include <fightingPit/contender/ContenderScripting.hh>
#include <fightingPit/team/AllyPartyTeams.hh>
#include <fightingPit/team/PartyTeam.hh>
#include <fightingPit/team/TeamMember.hh>

namespace fys::arena {

data::PriorityElem
SideBattle::getCurrentParticipantTurn(
		const std::chrono::system_clock::time_point& now,
		const std::chrono::milliseconds& timerInterlude)
{
	if (!_started) {
		_started = true;
	}
	else if (now < _endCurrentTurn && !_priorityOrderList.getCurrent().isContender) {
		return _priorityOrderList.getCurrent();
	}
	_endCurrentTurn = now + timerInterlude;
	return _priorityOrderList.getNext();
}

void
SideBattle::eraseFinishedAlterationAndDeadCharactersFromTurnList()
{
	for (auto deadContenders = _contenders.get().getDeadContenderOnSide(_side); auto& contender : deadContenders) {
		contender->accessStatus().cleanupFinishedAlteration();
		_priorityOrderList.removeParticipantFromList(contender->getContenderScripting()->getContenderId(), data::CONTENDER);
	}
	for (auto deadAllies = _partyTeams.get().getDeadMembersBySide(_side); auto& member : deadAllies) {
		member->accessStatus().cleanupFinishedAlteration();
		_priorityOrderList.removeParticipantFromList(member->getId(), data::PARTY_MEMBER);
	}
}

void
SideBattle::addParticipantInList(const TeamMember& tm)
{
	_priorityOrderList.addParticipantInList(tm.getId(), tm.getStatus().initialSpeed, false);
}

void
SideBattle::addParticipantInList(const FightingContender& contender)
{
	_priorityOrderList.addParticipantInList(contender.getId(), contender.getStatus().initialSpeed, true);
}

void
SideBattle::removeParticipantFromList(const TeamMember& tm)
{
	_priorityOrderList.removeParticipantFromList(tm.getId(), false);
}

void
SideBattle::removeParticipantFromList(const FightingContender& contender)
{
	_priorityOrderList.removeParticipantFromList(contender.getId(), true);
}

}
