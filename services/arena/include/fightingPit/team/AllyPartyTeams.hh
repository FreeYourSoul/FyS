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


#ifndef FYS_ALLYPARTYTEAMS_HH
#define FYS_ALLYPARTYTEAMS_HH

#include <vector>
#include <memory>
#include <fightingPit/team/PartyTeam.hh>

// forward declarations
namespace chaiscript {
    class ChaiScript;
}
namespace fys::arena {
    class PitContenders;
}

namespace fys::arena {

    template <typename T>
    using ComparatorSelection = std::function<bool(std::shared_ptr<T>, std::shared_ptr<T>)>;

    class AllyPartyTeams {

    public:
        void addPartyTeam(std::unique_ptr<PartyTeam> && team);

        [[nodiscard]] std::shared_ptr<TeamMember>
            selectSuitableMember(ComparatorSelection<TeamMember> comp);
        [[nodiscard]] std::shared_ptr<TeamMember>
            selectSuitableMemberOnSide(HexagonSide::Orientation side, ComparatorSelection<TeamMember> comp);
        [[nodiscard]] std::shared_ptr<TeamMember>
            selectSuitableMemberAlive(ComparatorSelection<TeamMember> comp);
        [[nodiscard]] std::shared_ptr<TeamMember>
            selectSuitableMemberOnSideAlive(HexagonSide::Orientation side, ComparatorSelection<TeamMember> comp);
        [[nodiscard]] std::shared_ptr<TeamMember>
            selectRandomMemberOnSideAlive(HexagonSide::Orientation side);
        [[nodiscard]] std::shared_ptr<TeamMember>
            selectMemberById(unsigned idMember);
        [[nodiscard]] std::vector<std::shared_ptr<TeamMember>>
            getMembersBySide(HexagonSide::Orientation side) const;

        [[nodiscard]] std::shared_ptr<TeamMember>
            getSpecificTeamMemberByName(const std::string & userName, const std::string & memberName) const;

        unsigned getNumberAlly() const { return _currentTeamMemberId; }
        unsigned allyNumberOnSide(HexagonSide::Orientation side) const;

        void executeAllyAction(const data::PriorityElem & ally, PitContenders & pc, std::unique_ptr<chaiscript::ChaiScript> & chaiPtr);

    private:
        std::vector<std::unique_ptr<PartyTeam>> _partyTeams;

        // automatically incremented for each TeamMember when adding a PartyTeam
        unsigned _currentTeamMemberId { 0 };

    };

}

#endif //FYS_ALLYPARTYTEAMS_HH
