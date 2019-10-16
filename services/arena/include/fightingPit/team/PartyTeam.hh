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


#ifndef FYS_PARTYTEAM_HH
#define FYS_PARTYTEAM_HH

#include <memory>
#include <vector>
#include <utility>
#include <fightingPit/HexagonSide.hh>

namespace fys::arena {

    //forward declarations
    class TeamMember;

    class PartyTeam {

    public:
        std::vector<std::shared_ptr<TeamMember>> getTeamMemberOnSide(HexagonSide::Orientation side);
        std::vector<std::shared_ptr<TeamMember>> getChangingSideTeamMember();

        void addPartyTeam(std::shared_ptr<TeamMember> member) {
            _members.emplace_back(std::move(member));
        }

    private:
        std::vector<std::shared_ptr<TeamMember> > _members;
        std::vector<bool> _changeSideFlags;

    };

    using PartyTeamUPtr = std::unique_ptr<PartyTeam>;

}

#endif //FYS_PARTYTEAM_HH
