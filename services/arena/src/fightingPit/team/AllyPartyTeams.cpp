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

#include <algorithm>

#include <algorithm/algorithm.hh>
#include <fightingPit/team/TeamMember.hh>
#include <fightingPit/team/PartyTeam.hh>
#include <fightingPit/team/AllyPartyTeams.hh>

namespace fys::arena {

    void AllyPartyTeams::addPartyTeam(std::unique_ptr<PartyTeam> && team)  {
        _partyTeams.emplace_back(std::move(team));
    }

    std::shared_ptr<TeamMember>
    AllyPartyTeams::selectSuitableMember(ComparatorSelection<TeamMember> comp) {
        if (_partyTeams.empty() || _partyTeams.front()->accessTeamMembers().empty())
            return nullptr;
        auto suitable = _partyTeams.front()->accessTeamMembers().begin();
        for (auto & _partyTeam : _partyTeams) {
            auto &teamMembers = _partyTeam->accessTeamMembers();
            auto suitableTmp = fys::find_most_suitable(teamMembers.begin(), teamMembers.end(), comp, suitable);

            if (suitableTmp != teamMembers.end())
                suitable = suitableTmp;
        }
        return *suitable;
    }

    std::shared_ptr<TeamMember>
    AllyPartyTeams::selectSuitableMemberAlive(ComparatorSelection<TeamMember> comp) {
        return selectSuitableMember([&comp](auto current, auto next) -> bool {
            return (!current->accessStatus().life.isDead()) && comp(current, next);
        });
    }

    std::shared_ptr<TeamMember>
    AllyPartyTeams::selectSuitableMemberOnSide(HexagonSide::Orientation side, ComparatorSelection<TeamMember> comp) {
        return selectSuitableMember([side, &comp](auto current, auto next) -> bool {
            return current->getHexagonSideOrient() == side && comp(current, next);
        });
    }

    std::shared_ptr<TeamMember>
    AllyPartyTeams::selectSuitableMemberOnSideAlive(HexagonSide::Orientation side, ComparatorSelection<TeamMember> comp) {
        return selectSuitableMember([side, &comp](auto current, auto next) -> bool {
            return (!current->accessStatus().life.isDead()) && current->getHexagonSideOrient() == side && comp(current, next);
        });
    }


}
