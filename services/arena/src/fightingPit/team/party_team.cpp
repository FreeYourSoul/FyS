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
#include <spdlog/spdlog.h>

#include <fightingPit/team/party_team.hh>
#include <fightingPit/team/team_member.hh>

namespace fys::arena {

std::vector<std::shared_ptr<team_member>>
party_team::team_member_on_side(fys::arena::hexagon_side::orientation orientation) const {
  std::vector<std::shared_ptr<team_member>> result;
  std::ranges::copy_if(_members, std::back_inserter(result), [orientation](const auto& tm_ptr) {
    return tm_ptr->side_orient() == orientation;
  });
  return result;
}

std::vector<std::shared_ptr<team_member>>
party_team::get_dead_team_members_on_side(fys::arena::hexagon_side::orientation side) const {
  std::vector<std::shared_ptr<team_member>> result;
  std::ranges::copy_if(_members, std::back_inserter(result), [side](const auto& tm_ptr) {
    return tm_ptr->side_orient() == side && tm_ptr->status().life_pt.is_dead();
  });
  return result;
}

void party_team::add_team_member(std::shared_ptr<team_member> member) {
  _members.emplace_back(std::move(member));
}

unsigned
party_team::ally_number_on_side(hexagon_side::orientation side) const {
  return std::ranges::count_if(_members, [side](const team_member_sptr& ally) { return side == ally->side_orient(); });
}

}// namespace fys::arena
