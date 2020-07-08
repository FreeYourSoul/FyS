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
#include <fightingPit/team/team_member.hh>
#include <fightingPit/team/party_team.hh>
#include <algorithm/algorithm.hh>

namespace fys::arena {

std::vector<std::shared_ptr<team_member>>
party_team::get_team_member_on_side(fys::arena::hexagon_side::orientation side) const
{
	std::vector<std::shared_ptr<team_member>> result;
	std::copy_if(_members.begin(), _members.end(), std::back_inserter(result), [side](const auto& contenderPtr) {
		return contenderPtr->get_hexagon_side_orient() == side;
	});
	return result;
}

std::vector<std::shared_ptr<team_member>>
party_team::get_dead_team_members_on_side(fys::arena::hexagon_side::orientation side) const
{
	std::vector<std::shared_ptr<team_member>> result;
	std::copy_if(_members.begin(), _members.end(), std::back_inserter(result), [side](const auto& contender_ptr) {
		return contender_ptr->get_hexagon_side_orient() == side && contender_ptr->get_status().life_pt.is_dead();
	});
	return result;
}

void
party_team::add_team_member(std::shared_ptr<team_member> member)
{
	_members.emplace_back(std::move(member));
}

unsigned
party_team::ally_number_on_side(hexagon_side::orientation side) const
{
	return std::count_if(_members.cbegin(), _members.cend(),
			[side](const team_member_sptr& ally) {
				return side == ally->get_hexagon_side_orient();
			}
	);
}

}
