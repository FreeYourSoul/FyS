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


#ifndef FYS_TEAMMEMBER_HH
#define FYS_TEAMMEMBER_HH

#include <variant>
#include <memory>
#include <fightingPit/HexagonSide.hh>
#include <fightingPit/data/CommonTypes.hh>
#include <SizedQueue.hh>

// forward declarations
namespace chaiscript {
    class ChaiScript;
}
namespace fys::arena {
    class AllyPartyTeams;
    class PitContenders;
}

namespace fys::arena {

    struct ContenderTargetId {
        uint v;
    };

    struct AllyTargetId {
        uint v;
    };

    /**
     * Pending actions of a team member are defined by
     * - the id of the action (mapped as index to the vector TeamMember::_actionDoable)
     * - the target, optional as every action doesn't require target.
     *   Can be a specific id (of an ally or a contender depending on the action)
     *   Can be a side (as some action can target a whole side)
     */
    struct PendingAction {
        uint idAction;
        std::optional<std::variant<ContenderTargetId, AllyTargetId, HexagonSide::Orientation> > target;
    };

    /**
     *
     */
    class TeamMember {

    public:
        TeamMember(std::string userName, std::string teamMemberName) :
                _userName(std::move(userName)), _name(std::move(teamMemberName)) {}

        void executeAction(AllyPartyTeams & apt, PitContenders & pc, std::unique_ptr<chaiscript::ChaiScript> & chaiPtr);
        void moveTeamMember(HexagonSide::Orientation destination, bool bypassCheck = false);
        void moveTeamMember(data::MoveDirection rightOrLeft);

        /**
         * @brief called by #fys::arena::AllyPartyTeam to set the id of the team member
         */
        void setId(unsigned id) { _id = id; }
        void addDoableAction(std::string doable, uint level) { _actionsDoable.emplace_back(std::move(doable), level); }

        [[nodiscard]] HexagonSide::Orientation getHexagonSideOrient() const { return (*_side).second; }
        [[nodiscard]] data::Status &accessStatus() { return _status; }
        [[nodiscard]] const HexagonSide &getHexagonSide() const { return _side; }
        [[nodiscard]] const data::Status &getStatus() const { return _status; }
        [[nodiscard]] const std::string &getUserName() const { return _userName; }
        [[nodiscard]] const std::string &getName() const { return _name; }
        [[nodiscard]] const std::vector<std::pair<std::string, uint>> &getActionsDoable() const { return _actionsDoable; }
        [[nodiscard]] unsigned getId() const { return _id; }

    private:
        std::string _userName;
        std::string _name;
        HexagonSide _side;
        unsigned _id {};
        data::Status _status;

        // action name with level of action
        std::vector<std::pair<std::string, uint>> _actionsDoable;
         fys::common::SizedQueue<PendingAction> _pendingActions;

    };

    using TeamMemberSPtr = std::shared_ptr<TeamMember>;

}

#endif //FYS_TEAMMEMBER_HH
