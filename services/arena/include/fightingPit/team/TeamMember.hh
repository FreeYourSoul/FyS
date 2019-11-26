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

#include <memory>
#include <fightingPit/HexagonSide.hh>
#include <fightingPit/data/CommonTypes.hh>

namespace fys::arena {

    class TeamMember {

    public:
        void moveTeamMember(HexagonSide::Orientation destination, bool bypassCheck = false);
        void moveTeamMember(data::MoveDirection rightOrLeft);

        void setName(const std::string &name) { _name = name; }

        [[nodiscard]] data::Status &accessStatus() { return _status; }
        [[nodiscard]] std::pair<HexagonSide::Hexagon, HexagonSide::Orientation> getHexagonSide() const { return *_side; }
        [[nodiscard]] HexagonSide::Orientation getHexagonSideOrient() const { return (*_side).second; }
        [[nodiscard]] const std::string &getName() const { return _name; }

    private:
        HexagonSide _side;
        std::string _name;
        data::Status _status;

        // PendingAction _pendingAction; Todo

    };

    using TeamMemberSPtr = std::shared_ptr<TeamMember>;

}

#endif //FYS_TEAMMEMBER_HH
