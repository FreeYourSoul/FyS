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
#include <fightingPit/team/TeamMember.hh>
#include <fightingPit/team/PartyTeam.hh>
#include <algorithm/algorithm.hh>

namespace fys::arena {

    std::vector<std::shared_ptr<TeamMember>> PartyTeam::getChangingSideTeamMember() const {
        std::vector<std::shared_ptr<TeamMember>> result;
        result.reserve(_changeSideFlags.size());
        for (std::size_t i = 0; i < _changeSideFlags.size(); ++i) {
            if (_changeSideFlags.at(i)) {
                result.emplace_back(_members.at(i));
            }
        }
        result.shrink_to_fit();
        return result;
    }

    std::vector<std::shared_ptr<TeamMember>> PartyTeam::getTeamMemberOnSide(fys::arena::HexagonSide::Orientation side) const {
        std::vector<std::shared_ptr<TeamMember>> result;
        std::copy_if(_members.begin(), _members.end(), std::back_inserter(result), [side](const auto &contenderPtr){
            return contenderPtr->getHexagonSideOrient() == side;
        });
        return result;
    }

    void PartyTeam::addTeamMember(std::shared_ptr<TeamMember> member) {
        _members.emplace_back(std::move(member));
    }

    void PartyTeam::addPendingActionToTeamMember(unsigned id) {
        auto it = std::find_if(_members.begin(), _members.end(), [id](const auto & teamMember) {
            return id == teamMember->getId();
        });
        if (it != _members.end()) {
//            it->get()->
        }
        else {
            SPDLOG_ERROR("Adding pending action to a non existing team member of id {}", id);
        }
    }

    unsigned PartyTeam::allyNumberOnSide(HexagonSide::Orientation side) const {
        return std::count_if(_members.cbegin(), _members.cend(),
                 [side](const TeamMemberSPtr & ally) {
                     return side == ally->getHexagonSideOrient();
                 }
        );
    }

}
