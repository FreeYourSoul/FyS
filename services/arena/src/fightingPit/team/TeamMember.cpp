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

#include <fmt/ostream.h>
#include <spdlog/spdlog.h>
#include <chaiscript/chaiscript.hpp>
#include <fightingPit/data/CommonTypes.hh>
#include <fightingPit/contender/PitContenders.hh>
#include <fightingPit/contender/FightingContender.hh>
#include <fightingPit/team/TeamMember.hh>
#include <fightingPit/team/AllyPartyTeams.hh>

// overloaded trick
template<class... Ts>
struct overloaded : Ts ... {
    using Ts::operator()...;
};
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace fys::arena {

    void TeamMember::moveTeamMember(HexagonSide::Orientation destination, bool bypassCheck) {
        if (!_side.move(destination, bypassCheck)) {
            SPDLOG_ERROR("Impossible move from {} to {}", _side, destination);
            return;
        }
    }

    void TeamMember::moveTeamMember(data::MoveDirection directionToMove) {
        if (directionToMove == data::MoveDirection::RIGHT) {
            if (!_side.moveRight()) {
                SPDLOG_ERROR("TeamMember {}::{} Impossible move from {} to right", _userName, _name, _side);
            }
        } else if (directionToMove == data::MoveDirection::LEFT) {
            if (!_side.moveLeft()) {
                SPDLOG_ERROR("TeamMember {}::{} Impossible move from {} to left", _userName, _name, _side);
            }
        } else if (directionToMove == data::MoveDirection::BACK) {
            if (!_side.moveBack()) {
                SPDLOG_ERROR("TeamMember {}::{} Impossible move from {} to backside", _userName, _name, _side);
            }
        }
    }

    void TeamMember::executeAction(
            AllyPartyTeams &apt,
            PitContenders &pc,
            std::unique_ptr<chaiscript::ChaiScript> &chaiPtr) {
        auto pa = _pendingActions.pop();

        if (!pa) {
            SPDLOG_DEBUG("No action to execute in the pipeline for team member {}::{} id {}", _userName, _name, _id);
            return;
        }
        if (pa->idAction >= _actionsDoable.size()) {
            SPDLOG_ERROR("TeamMember {}::{} id {} tried to execute a non existing action of id {}",
                         _userName, _name, _id, pa->idAction);
            return;
        }

        const std::string allyAction = fmt::format(R"(ally_actions["{}_{}"]["{}"])",
                                                   _userName, _name, _actionsDoable.at(pa->idAction).first);
        auto funcAction = chaiPtr->eval<std::function<int(data::Status)>>(fmt::format(
                R"(fun(allyStatus){{ return {}.execute(allyStatus);}})", allyAction));
        auto targetType = chaiPtr->eval<data::Targeting>(allyAction + ".requireTarget();");

        try {
            const auto &targetStatus = _status;
            if (pa->target) {
                std::visit(overloaded{
                        [&apt, &pc, &targetType, &funcAction](AllyTargetId target) {
                            if (targetType == data::ALLY || targetType == data::ALLY_OR_ENNEMY)
                                funcAction(apt.selectMemberById(target.v)->getStatus());
                        },

                        [&apt, &pc, &targetType, &funcAction](ContenderTargetId target) {
                            if (targetType == data::ENNEMY || targetType == data::ALLY_OR_ENNEMY)
                                funcAction(pc.getFightingContender(target.v)->getStatus());
                        },

                        [&apt, &pc, &funcAction](auto o) {
                            // NOT SUPPORTED YET
                        },
                }, *pa->target);
            } else if (funcAction(_status)) {
                SPDLOG_DEBUG("Ally {}::{} id {} executed action {}", _userName, _name, _id, pa->idAction);
            }
        }
        catch (const chaiscript::exception::eval_error &ee) {
            SPDLOG_ERROR("Error caught on script execution while executing {} with target required.\n"
                         "Team owned by {} TeamMember {}\n{}",
                         pa->idAction, static_cast<bool>(pa->target), _userName, _name, ee.what());
        }
    }

    void TeamMember::addPendingAction(const std::string &actionName) {

    }

}
