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

#include <algorithm>
#include <numeric>
#include <spdlog/spdlog.h>

#include <chaiscript/chaiscript.hpp>

#include <fil/algorithm/suitable.hh>

#include <random_generator.hh>

#include <fightingPit/contender/pit_contenders.hh>
#include <fightingPit/team/ally_party_teams.hh>
#include <fightingPit/team/party_team.hh>
#include <fightingPit/team/team_member.hh>
#include <history_manager.hh>

namespace fys::arena {

ally_party_teams::~ally_party_teams() = default;

bool ally_party_teams::execute_ally_action(
    const data::priority_elem& ally,
    pit_contenders& pc,
    std::unique_ptr<chaiscript::ChaiScript>& chai_ptr) {
  if (ally.is_contender) {
    return false;
  }
  if (auto member = select_member_by_id(ally.id); !member) {
    SPDLOG_ERROR("Member with id {} isn't found, action cannot be executed", ally.id);
    return false;
  } else {
    bool turn_executed = true;
    const bool is_turn_processed = member->access_status().process_alteration_before_turn();

    if (is_turn_processed) {
      turn_executed = member->execute_action(*this, pc, chai_ptr);
    }
    member->access_status().process_alteration_after_turn();
    return turn_executed;
  }
}

void ally_party_teams::add_party_team(std::unique_ptr<party_team>&& team) {
  for (auto& tm : team->access_team_members()) {
    tm->set_id(++_current_team_member_id);
  }
  _party_teams.emplace_back(std::move(team));
}

std::shared_ptr<team_member>
ally_party_teams::select_suitable_member(comparator_selection<team_member> comp) {
  bool found = false;
  if (_party_teams.empty() || _party_teams.front()->team_members().empty())
    return nullptr;
  auto suitable = _party_teams.front()->team_members().begin();
  for (auto& party_team : _party_teams) {
    auto& team_members = party_team->team_members();
    auto suitable_tmp = fil::find_most_suitable(team_members.begin(), team_members.end(), comp, suitable);

    if (suitable_tmp != team_members.end()) {
      found = true;
      suitable = suitable_tmp;
    }
  }
  return (found) ? *suitable : nullptr;
}

std::shared_ptr<team_member>
ally_party_teams::select_random_member_on_side_alive(hexagon_side::orientation side) {
  auto members_on_side = members_by_side(side);
  std::uint32_t random_index = fys::util::random_generator::generate_in_range(1ul, members_on_side.size());
  return members_on_side.at(random_index - 1);
}

std::shared_ptr<team_member>
ally_party_teams::select_suitable_member_alive(comparator_selection<team_member> comp) {
  return select_suitable_member([&comp](auto current, auto next) -> bool {
    return !current->access_status().life_pt.is_dead() && !next->access_status().life_pt.is_dead() && comp(current, next);
  });
}

std::shared_ptr<team_member>
ally_party_teams::select_suitable_member_on_side(hexagon_side::orientation side, comparator_selection<team_member> comp) {
  return select_suitable_member([side, &comp](auto current, auto next) -> bool {
    return current->side_orient() == side && next->side_orient() == side && comp(current, next);
  });
}

std::shared_ptr<team_member>
ally_party_teams::select_suitable_member_on_side_alive(hexagon_side::orientation side, comparator_selection<team_member> comp) {
  return select_suitable_member([side, &comp](auto current, auto next) -> bool {
    return (!next->status().life_pt.is_dead() && !current->status().life_pt.is_dead() && current->side_orient() == side && next->side_orient() == side) && comp(current, next);
  });
}

std::shared_ptr<team_member>
ally_party_teams::get_specific_team_member_by_id(const std::string& user_name, unsigned id_member) const {
  auto it_team = std::ranges::find_if(_party_teams, [&user_name](const auto& team) {
    return team->user_name() == user_name;
  });

  if (it_team != _party_teams.cend()) {
    auto it_member = std::ranges::find_if((*it_team)->team_members(), [id_member](const auto& member) {
      return member->id() == id_member;
    });

    if (it_member != (*it_team)->team_members().cend())
      return *it_member;
  }
  return nullptr;
}

std::shared_ptr<team_member>
ally_party_teams::select_member_by_id(unsigned id_member) {
  if (_party_teams.empty() || _party_teams.front()->access_team_members().empty())
    return nullptr;
  for (auto& partyTeam : _party_teams) {
    const auto& team_members = partyTeam->access_team_members();
    auto team_member = std::ranges::find_if(team_members, [id_member](const auto& tm) { return id_member == tm->id(); });

    if (team_member != team_members.end())
      return *team_member;
  }
  return nullptr;
}

unsigned ally_party_teams::ally_number_on_side(hexagon_side::orientation side) const {
  return std::accumulate(_party_teams.cbegin(), _party_teams.cend(), 0u, [side](unsigned count, const party_team_uptr& party) {
    return count + party->ally_number_on_side(side);
  });
}

std::vector<std::shared_ptr<team_member>>
ally_party_teams::members_by_side(hexagon_side::orientation side) const {
  std::vector<std::shared_ptr<team_member>> ret;
  if (_party_teams.empty() || _party_teams.front()->team_members().empty())
    return ret;
  for (const auto& party_team : _party_teams) {
    const auto& members_on_side = party_team->team_member_on_side(side);
    ret.reserve(members_on_side.size());
    std::ranges::copy(members_on_side, std::back_inserter(ret));
  }
  return ret;
}

std::vector<std::shared_ptr<team_member>>
ally_party_teams::dead_members_by_side(hexagon_side::orientation side) const {
  std::vector<std::shared_ptr<team_member>> ret;
  if (_party_teams.empty() || _party_teams.front()->team_members().empty()) {
    return ret;
  }
  for (const auto& party_team : _party_teams) {
    const auto& members_on_side = party_team->get_dead_team_members_on_side(side);
    ret.reserve(members_on_side.size());
    std::ranges::copy(members_on_side, std::back_inserter(ret));
  }
  return ret;
}

const party_team&
ally_party_teams::get_party_team_of_player(const std::string& user_name) const {
  auto it = std::ranges::find_if(_party_teams, [&user_name](const auto& party_team) {
    return user_name == party_team->user_name();
  });
  if (it == _party_teams.end()) {
    throw std::runtime_error(fmt::format("GetPartyTeamOfPlayer called on a non-existing player of name {}", user_name));
  }
  return **it;
}

bool ally_party_teams::set_party_readiness(const std::string& user_name) {
  auto it = std::ranges::find_if(_party_teams, [&user_name](const auto& party_team) {
    return user_name == party_team->user_name();
  });
  if (it == _party_teams.end()) {
    throw std::runtime_error(fmt::format("GetPartyTeamOfPlayer called on a non-existing player of name {}", user_name));
  }

  // do not process if the team has been already set to ready
  if ((*it)->is_team_ready()) {
    SPDLOG_WARN("Player '{}' is setting himself ready twice", user_name);
    return false;
  }

  (*it)->set_team_ready(true);
  return std::ranges::all_of(_party_teams, [](const auto& party_team) { return party_team->is_team_ready(); });
}
bool ally_party_teams::all_dead() const {
  return std::ranges::all_of(_party_teams, [](const auto& pt) {
    return std::ranges::all_of(pt->team_members(), [](const auto& member) { return member->status().life_pt.is_dead(); });
  });
}
const std::vector<std::unique_ptr<party_team>>&
ally_party_teams::get_party_teams() const { return _party_teams; }

}// namespace fys::arena
