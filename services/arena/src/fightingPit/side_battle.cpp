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

#include <fightingPit/side_battle.hh>
#include <fightingPit/contender/pit_contenders.hh>
#include <fightingPit/contender/fighting_contender.hh>
#include <fightingPit/contender/contender_scripting.hh>
#include <fightingPit/team/ally_party_teams.hh>
#include <fightingPit/team/party_team.hh>
#include <fightingPit/team/team_member.hh>

namespace fys::arena {

data::priority_elem
side_battle::get_current_participant_turn(
		const std::chrono::system_clock::time_point& now,
		const std::chrono::milliseconds& timer_interlude)
{
	if (_turnDone) {
		_turnDone = false;
	}
	else if (now < _endCurrentTurn && !_priority_order_list.get_current().is_contender) {
		return _priority_order_list.get_current();
	}
	_endCurrentTurn = now + timer_interlude;
	return _priority_order_list.get_next();
}

void
side_battle::erase_finished_alteration_and_dead_characters_from_turn_list()
{
	for (auto dead_contenders = _contenders.get().get_dead_contender_on_side(_side); auto& contender : dead_contenders) {
		contender->access_status().cleanup_finished_alteration();
		_priority_order_list.rm_participant_from_list(contender->get_contender_scripting()->get_contender_id(), data::CONTENDER);
	}
	for (auto dead_allies = _partyTeams.get().get_dead_members_by_side(_side); auto& member : dead_allies) {
		member->access_status().cleanup_finished_alteration();
		_priority_order_list.rm_participant_from_list(member->get_id(), data::PARTY_MEMBER);
	}
}

void
side_battle::addParticipantInList(const team_member& tm)
{
	_priority_order_list.add_participant_in_list(tm.get_id(), tm.get_status().initial_speed, false);
}

void
side_battle::addParticipantInList(const fighting_contender& contender)
{
	_priority_order_list.add_participant_in_list(contender.get_id(), contender.get_status().initial_speed, true);
}

void
side_battle::removeParticipantFromList(const team_member& tm)
{
	_priority_order_list.rm_participant_from_list(tm.get_id(), false);
}

void
side_battle::removeParticipantFromList(const fighting_contender& contender)
{
	_priority_order_list.rm_participant_from_list(contender.get_id(), true);
}

}
