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

#include <spdlog/spdlog.h>
#include <algorithm>
#include <numeric>

#include <chaiscript/chaiscript.hpp>
#include <algorithm/algorithm.hh>

#include <random_generator.hh>

#include <fightingPit/team/team_member.hh>
#include <fightingPit/team/party_team.hh>
#include <fightingPit/team/ally_party_teams.hh>
#include <fightingPit/contender/pit_contenders.hh>
#include <history_manager.hh>

namespace fys::arena {

bool
ally_party_teams::execute_ally_action(
		const data::priority_elem& ally,
		pit_contenders& pc,
		std::unique_ptr<chaiscript::ChaiScript>& chai_ptr)
{
	if (ally.is_contender) {
		return false;
	}
	if (auto member = select_member_by_id(ally.id); !member) {
		SPDLOG_ERROR("Member with id {} isn't found, action cannot be executed", ally.id);
		return false;
	}
	else {
		bool turnExecuted = true;
		const bool isTurnProcessed = member->access_status().process_alteration_before_turn();

		if (isTurnProcessed) {
			turnExecuted = member->execute_action(*this, pc, chai_ptr);
		}
		member->access_status().process_alteration_after_turn();
		return turnExecuted;
	}
}

void
ally_party_teams::add_party_team(std::unique_ptr<party_team>&& team)
{
	for (auto& tm : team->access_team_members()) {
		tm->set_id(++_current_team_member_id);
	}
	_party_teams.emplace_back(std::move(team));
}

std::shared_ptr<team_member>
ally_party_teams::select_suitable_member(comparator_selection<team_member> comp)
{
	bool found = false;
	if (_party_teams.empty() || _party_teams.front()->get_team_members().empty())
		return nullptr;
	auto suitable = _party_teams.front()->get_team_members().begin();
	for (auto& partyTeam : _party_teams) {
		auto& teamMembers = partyTeam->get_team_members();
		auto suitableTmp = fys::find_most_suitable(teamMembers.begin(), teamMembers.end(), comp, suitable);

		if (suitableTmp != teamMembers.end()) {
			found = true;
			suitable = suitableTmp;
		}
	}
	return (found) ? *suitable : nullptr;
}

std::shared_ptr<team_member>
ally_party_teams::select_random_member_on_side_alive(hexagon_side::orientation side)
{
	auto membersOnSide = get_members_by_side(side);
	uint randomIndex = fys::util::random_generator::generate_in_range(1ul, membersOnSide.size());
	return membersOnSide.at(randomIndex - 1);
}

std::shared_ptr<team_member>
ally_party_teams::select_suitable_member_alive(comparator_selection<team_member> comp)
{
	return select_suitable_member([&comp](auto current, auto next) -> bool {
		return !current->accessStatus().life.isDead() && !next->accessStatus().life.isDead() && comp(current, next);
	});
}

std::shared_ptr<team_member>
ally_party_teams::select_suitable_member_on_side(hexagon_side::orientation side, comparator_selection<team_member> comp)
{
	return select_suitable_member([side, &comp](auto current, auto next) -> bool {
		return current->getHexagonSideOrient() == side && next->getHexagonSideOrient() == side &&
				comp(current, next);
	});
}

std::shared_ptr<team_member>
ally_party_teams::select_suitable_member_on_side_alive(hexagon_side::orientation side, comparator_selection<team_member> comp)
{
	return select_suitable_member([side, &comp](auto current, auto next) -> bool {
		return (!next->getStatus().life.isDead() && !current->getStatus().life.isDead() &&
				current->getHexagonSideOrient() == side && next->getHexagonSideOrient() == side) &&
				comp(current, next);
	});
}

std::shared_ptr<team_member>
ally_party_teams::get_specific_team_member_by_id(const std::string& user_name, unsigned id_member) const
{
	auto itTeam = std::find_if(_party_teams.cbegin(), _party_teams.cend(),
			[&user_name](const auto& team) {
				return team->getUserName() == user_name;
			});

	if (itTeam != _party_teams.cend()) {
		auto itMember = std::find_if((*itTeam)->get_team_members().cbegin(), (*itTeam)->get_team_members().cend(),
				[id_member](const auto& member) {
					return member->getId() == id_member;
				});

		if (itMember != (*itTeam)->get_team_members().cend())
			return *itMember;
	}
	return nullptr;
}

std::shared_ptr<team_member>
ally_party_teams::select_member_by_id(unsigned id_member)
{
	if (_party_teams.empty() || _party_teams.front()->access_team_members().empty())
		return nullptr;
	for (auto& partyTeam : _party_teams) {
		const auto& teamMembers = partyTeam->access_team_members();
		auto teamMember = std::find_if(teamMembers.begin(), teamMembers.end(), [id_member](const auto& tm) {
			return id_member == tm->getId();
		});

		if (teamMember != teamMembers.end())
			return *teamMember;
	}
	return nullptr;
}

unsigned
ally_party_teams::ally_number_on_side(hexagon_side::orientation side) const
{
	return std::accumulate(_party_teams.cbegin(), _party_teams.cend(), 0u,
			[side](unsigned count, const party_team_uptr& party) {
				return count + party->ally_number_on_side(side);
			}
	);
}

std::vector<std::shared_ptr<team_member>>
ally_party_teams::get_members_by_side(hexagon_side::orientation side) const
{
	std::vector<std::shared_ptr<team_member>> ret;
	if (_party_teams.empty() || _party_teams.front()->get_team_members().empty())
		return ret;
	for (const auto& partyTeam : _party_teams) {
		const auto& membersOnSide = partyTeam->get_team_member_on_side(side);
		std::copy(membersOnSide.begin(), membersOnSide.end(), std::back_inserter(ret));
	}
	return ret;
}

std::vector<std::shared_ptr<team_member>>
ally_party_teams::get_dead_members_by_side(hexagon_side::orientation side) const
{
	std::vector<std::shared_ptr<team_member>> ret;
	if (_party_teams.empty() || _party_teams.front()->get_team_members().empty())
		return ret;
	for (const auto& partyTeam : _party_teams) {
		const auto& membersOnSide = partyTeam->get_dead_team_members_on_side(side);
		std::copy(membersOnSide.begin(), membersOnSide.end(), std::back_inserter(ret));
	}
	return ret;
}

const party_team&
ally_party_teams::get_party_team_of_player(const std::string& user_name) const
{
	auto it = std::find_if(_party_teams.begin(), _party_teams.end(), [&user_name](const auto& partyTeam) {
		return user_name == partyTeam->getUserName();
	});
	if (it == _party_teams.end()) {
		throw std::runtime_error(fmt::format("GetPartyTeamOfPlayer called on a non-existing player of name {}", user_name));
	}
	return **it;
}

bool
ally_party_teams::set_party_readiness(const std::string& user_name)
{
	auto it = std::find_if(_party_teams.begin(), _party_teams.end(), [&user_name](const auto& partyTeam) {
		return user_name == partyTeam->getUserName();
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
	return std::all_of(_party_teams.begin(), _party_teams.end(), [](const auto& partyTeam) {
		return partyTeam->isTeamReady();
	});
}
bool
ally_party_teams::all_dead() const
{
	return std::all_of(_party_teams.begin(), _party_teams.end(), [](const auto& pt) {
		return std::all_of(pt->getTeamMembers().begin(), pt->getTeamMembers().end(), [](const auto& member) {
			return member->getStatus().life.isDead();
		});
	});
}

}
