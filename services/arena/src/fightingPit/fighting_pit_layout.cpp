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

#include <fightingPit/contender/pit_contenders.hh>
#include <fightingPit/team/ally_party_teams.hh>
#include <fightingPit/fighting_pit_layout.hh>
#include <fightingPit/team/team_member.hh>
#include <fightingPit/contender/contender_scripting.hh>
#include <fightingPit/contender/fighting_contender.hh>
#include <fightingPit/side_battle.hh>

namespace fys::arena {

fighting_pit_layout::fighting_pit_layout(pit_contenders& contenders, ally_party_teams& partyTeams)
		:_contenders(contenders), _party_teams(partyTeams)
{
	for (auto& pt : partyTeams.get_party_teams()) {
		add_active_party_team(*pt);
	}
	add_active_contender(contenders.getNumberContender());
}

void
fighting_pit_layout::add_active_contender(uint number_contender_to_add)
{
	_moving_flag_contender.resize(_moving_flag_contender.size() + number_contender_to_add, false);
}

void
fighting_pit_layout::add_active_party_team(const party_team& pt)
{
	for (auto& tm : pt.get_team_members()) {
		_moving_flag_ally[std::pair(tm->get_user_name(), tm->get_id())] = false;
	}
}

unsigned
fighting_pit_layout::active_characters_on_side(hexagon_side::orientation side) const
{
	return _contenders.get().contenderOnSide(side) + _party_teams.get().ally_number_on_side(side);
}

void
fighting_pit_layout::execute_movements(std::vector<side_battle>& sides)
{
	auto changingSideContenders = get_changing_side_contenders();
	auto changingSideAlly = get_changing_side_team_members();
	auto movementExecution = [&sides](auto& characterToMove) {
		hexagon_side::orientation sideBeforeMove = (*characterToMove->_side).second;
		hexagon_side::orientation sideAfterMove = characterToMove->_moving;
		if (characterToMove->_side.move(sideAfterMove, true)) {
			sides[static_cast<uint>(sideBeforeMove)].removeParticipantFromList(*characterToMove);
			sides[static_cast<uint>(sideAfterMove)].addParticipantInList(*characterToMove);
		}
	};

	std::for_each(changingSideAlly.begin(), changingSideAlly.end(), movementExecution);
	std::for_each(changingSideContenders.begin(), changingSideContenders.end(), movementExecution);
}

std::vector<std::shared_ptr<team_member>>
fighting_pit_layout::get_changing_side_team_members()
{
	std::vector<std::shared_ptr<team_member>> result;

	for (auto&[userNameId, isMoving] : _moving_flag_ally) {
		if (isMoving) {
			auto&[userName, id] = userNameId;
			result.emplace_back(_party_teams.get().get_specific_team_member_by_id(userName, id));
		}
		isMoving = false;
	}
	return result;
}

std::vector<std::shared_ptr<fighting_contender>>
fighting_pit_layout::get_changing_side_contenders()
{
	std::vector<std::shared_ptr<fighting_contender>> result;

	for (uint i = 0; i < _moving_flag_contender.size(); ++i) {
		if (_moving_flag_contender.at(i) == true) {
			result.emplace_back(_contenders.get().getFightingContender(i));
		}
		_moving_flag_contender[i] = false;
	}
	return result;
}

void
fighting_pit_layout::set_contender_initiate_position(fighting_contender& contender, hexagon_side::orientation side)
{
	if ((*contender._side).second != hexagon_side::orientation::NONE) {
		SPDLOG_ERROR("Can't set initial position for contender {} as position is not NONE", contender.get_name());
		return;
	}
	contender._side.move(side, true);
}

void
fighting_pit_layout::set_ally_move_initiate_position(team_member& tm, hexagon_side::orientation side)
{
	if ((*tm._side).second != hexagon_side::orientation::NONE) {
		SPDLOG_ERROR("Can't set initial position for ally {}.{} as position is not NONE", tm.get_user_name(), tm.get_name());
		return;
	}
	tm._side.move(side, true);
}

bool
fighting_pit_layout::initiate_contender_move(std::shared_ptr<fighting_contender> contender, hexagon_side::orientation move_to)
{
	if (contender->getHexagonSide().can_move(move_to)) {
		contender->_moving = move_to;
		_moving_flag_contender[contender->get_id()] = true;
		return true;
	}
	return false;
}

void
fighting_pit_layout::initiate_force_contender_move(std::shared_ptr<fighting_contender> contender, hexagon_side::orientation move_to)
{
	contender->_moving = move_to;
	_moving_flag_contender[contender->get_id()] = true;
}

void
fighting_pit_layout::initiate_contender_move_dir(std::shared_ptr<fighting_contender> contender, data::move_direction move_dir)
{
	switch (move_dir) {
		case data::move_direction::BACK:
			initiate_contender_move(contender, contender->getHexagonSide().find_back());
			break;
		case data::move_direction::RIGHT:
			initiate_contender_move(contender, contender->getHexagonSide().find_right());
			break;
		case data::move_direction::LEFT:
			initiate_contender_move(contender, contender->getHexagonSide().find_left());
			break;
	}
}

bool
fighting_pit_layout::initiate_member_move(std::shared_ptr<team_member> member, hexagon_side::orientation move_to)
{
	if (member->get_hexagon_side().can_move(move_to)) {
		member->_status.moving = move_to;
		_moving_flag_contender[member->get_id()] = true;
		return true;
	}
	return false;
}

void
fighting_pit_layout::initiate_force_member_move(std::shared_ptr<team_member> member, hexagon_side::orientation move_to)
{
	member->_status.moving = move_to;
	_moving_flag_contender[member->get_id()] = true;
}

void
fighting_pit_layout::initiate_member_move_dir(std::shared_ptr<team_member> member, data::move_direction move_dir)
{
	switch (move_dir) {
		case data::move_direction::BACK:
			initiate_member_move(member, member->get_hexagon_side().find_back());
			break;
		case data::move_direction::RIGHT:
			initiate_member_move(member, member->get_hexagon_side().find_right());
			break;
		case data::move_direction::LEFT:
			initiate_member_move(member, member->get_hexagon_side().find_left());
			break;
	}
}

}