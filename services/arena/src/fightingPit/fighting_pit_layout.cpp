// MIT License
//
// Copyright (c) 2021-2022 Quentin Balland
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

#include <fmt/format.h>

#include <utility>

#include <fightingPit/contender/contender_scripting.hh>
#include <fightingPit/contender/fighting_contender.hh>
#include <fightingPit/contender/pit_contenders.hh>
#include <fightingPit/team/ally_party_teams.hh>
#include <fightingPit/team/party_team.hh>
#include <fightingPit/team/team_member.hh>

#include <fightingPit/fighting_pit_layout.hh>
#include <fightingPit/side_battle.hh>

#include <logger.hh>

namespace fys::arena {

fighting_pit_layout::fighting_pit_layout(pit_contenders& contenders, ally_party_teams& partyTeams)
    : _contenders(contenders), _party_teams(partyTeams) {
  for (auto& pt : partyTeams.get_party_teams()) {
    add_active_party_team(*pt);
  }
  add_active_contender(contenders.number_contender());
}

void fighting_pit_layout::add_active_contender(std::uint32_t number_contender_to_add) {
  _moving_flag_contender.resize(_moving_flag_contender.size() + number_contender_to_add, false);
}

void fighting_pit_layout::add_active_party_team(const party_team& pt) {
  for (auto& tm : pt.team_members()) {
    _moving_flag_ally[std::pair(tm->user_name(), tm->id())] = false;
  }
}

unsigned
fighting_pit_layout::active_characters_on_side(hexagon_side::orientation side) const {
  return _contenders.get().number_contender_on_side(side) + _party_teams.get().ally_number_on_side(side);
}

void fighting_pit_layout::execute_movements(std::vector<side_battle>& sides) {
  auto csc = changing_side_contenders();
  auto csa = get_changing_side_team_members();
  auto movement_execution = [&sides](auto& characterToMove) {
    hexagon_side::orientation sideBeforeMove = (*characterToMove->_side).second;
    hexagon_side::orientation sideAfterMove = characterToMove->_moving;
    if (characterToMove->_side.move(sideAfterMove, true)) {
      sides[static_cast<uint>(sideBeforeMove)].rm_participant_from_list(*characterToMove);
      sides[static_cast<uint>(sideAfterMove)].add_participant_in_list(*characterToMove);
    }
  };

  std::for_each(csa.begin(), csa.end(), movement_execution);
  std::for_each(csc.begin(), csc.end(), movement_execution);
}

std::vector<std::shared_ptr<team_member>>
fighting_pit_layout::get_changing_side_team_members() {
  std::vector<std::shared_ptr<team_member>> result;

  for (auto& [user_name_id, is_moving] : _moving_flag_ally) {
    if (is_moving) {
      auto& [user_name, id] = user_name_id;
      result.emplace_back(_party_teams.get().get_specific_team_member_by_id(user_name, id));
    }
    is_moving = false;
  }
  return result;
}

std::vector<std::shared_ptr<fighting_contender>>
fighting_pit_layout::changing_side_contenders() {
  std::vector<std::shared_ptr<fighting_contender>> result;

  for (std::uint32_t i = 0; i < _moving_flag_contender.size(); ++i) {
    if (_moving_flag_contender.at(i) == true) {
      result.emplace_back(_contenders.get().fighting_contender_at(i));
    }
    _moving_flag_contender[i] = false;
  }
  return result;
}

void fighting_pit_layout::set_contender_initiate_position(fighting_contender& contender, hexagon_side::orientation side) {
  if ((*contender._side).second != hexagon_side::orientation::NONE) {
    log_error(fmt::format("Can't set initial position for contender {} as position is not NONE", contender.name()));
    return;
  }
  contender._side.move(side, true);
}

void fighting_pit_layout::set_ally_move_initiate_position(team_member& tm, hexagon_side::orientation side) {
  if ((*tm._side).second != hexagon_side::orientation::NONE) {
    log_error(fmt::format("Can't set initial position for ally {}.{} as position is not NONE", tm.user_name(), tm.name()));
    return;
  }
  tm._side.move(side, true);
}

bool fighting_pit_layout::initiate_contender_move(std::shared_ptr<fighting_contender> contender, hexagon_side::orientation move_to) {
  if (contender->side().can_move_to(move_to)) {
    contender->_moving = move_to;
    _moving_flag_contender[contender->id()] = true;
    return true;
  }
  return false;
}

void fighting_pit_layout::initiate_force_contender_move(std::shared_ptr<fighting_contender> contender, hexagon_side::orientation move_to) {
  contender->_moving = move_to;
  _moving_flag_contender[contender->id()] = true;
}

void fighting_pit_layout::initiate_contender_move_dir(std::shared_ptr<fighting_contender> contender, data::move_direction move_dir) {
  switch (move_dir) {
  case data::move_direction::BACK:
    initiate_contender_move(contender, contender->side().find_back());
    break;
  case data::move_direction::RIGHT:
    initiate_contender_move(contender, contender->side().find_right());
    break;
  case data::move_direction::LEFT:
    initiate_contender_move(contender, contender->side().find_left());
    break;
  }
}

bool fighting_pit_layout::initiate_member_move(std::shared_ptr<team_member> member, hexagon_side::orientation move_to) {
  if (member->side().can_move_to(move_to)) {
    member->_status.moving = move_to;
    _moving_flag_contender[member->id()] = true;
    return true;
  }
  return false;
}

void fighting_pit_layout::initiate_force_member_move(std::shared_ptr<team_member> member, hexagon_side::orientation move_to) {
  member->_status.moving = move_to;
  _moving_flag_contender[member->id()] = true;
}

void fighting_pit_layout::initiate_member_move_dir(std::shared_ptr<team_member> member, data::move_direction move_dir) {
  switch (move_dir) {
  case data::move_direction::BACK:
    initiate_member_move(member, member->side().find_back());
    break;
  case data::move_direction::RIGHT:
    initiate_member_move(member, member->side().find_right());
    break;
  case data::move_direction::LEFT:
    initiate_member_move(member, member->side().find_left());
    break;
  }
}

}// namespace fys::arena