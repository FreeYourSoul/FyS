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
#include <fightingPit/contender/FightingContender.hh>
#include <fightingPit/FightingPit.hh>
#include <fightingPit/team/TeamMember.hh>

namespace {
    std::chrono::milliseconds retrieveTimeInterludeFromLevelDegree(fys::arena::FightingPit::Level level) {
        switch (level) {
            case fys::arena::FightingPit::Level::EASY:
                return std::chrono::milliseconds {20000};
            case fys::arena::FightingPit::Level::MEDIUM :
                return std::chrono::milliseconds {10000};
            case fys::arena::FightingPit::Level::HARD :
                return std::chrono::milliseconds {5000};
            default:
                SPDLOG_ERROR("Incorrect level");
                return std::chrono::milliseconds {0};
        }
    }
}

namespace fys::arena {

    FightingPit::FightingPit(std::string creatorUserName, fys::arena::FightingPit::Level levelFightingPit) :
            _end(Ending::ON_HOLD),
            _levelFightingPit(levelFightingPit),
            _timeInterlude(retrieveTimeInterludeFromLevelDegree(_levelFightingPit)),
            _layoutMapping(_contenders, _partyTeams),
            _creatorUserName(std::move(creatorUserName)),
            _chaiPtr(ChaiRegister::createChaiInstance(_contenders, _partyTeams))
    {}

    bool FightingPit::checkEndStatusFightingPit() {
        switch (_end) {
            case Ending::NOT_FINISHED:
                return true;
            case Ending::ON_HOLD:
                return false;
            case Ending::ALLY_WIN:
                // todo Send failure of the fight, close the fight properly (release resource if any)
                return false;
            case Ending::CONTENDER_WIN:
                // todo Send failure of the fight, close the fight properly (release resource if any)
                return false;
        }
        return _end == Ending::ON_HOLD;
    }

    void FightingPit::continueBattle(const std::chrono::system_clock::time_point & now) {
        if (checkEndStatusFightingPit())
            return;
        for (auto & side : _sideBattles) {
            auto currentParticipant = side.getCurrentParticipantTurn(now, _timeInterlude);

            if (currentParticipant.isContender) {
                // non-playable character (enemy NPC)
                _contenders.executeContenderAction(currentParticipant);
            } else {
                // character of a player
                _partyTeams.executeAllyAction(currentParticipant, _contenders, _chaiPtr);
            }
        }
    }

    void FightingPit::forwardMessageToTeamMember(unsigned idMember) {
        auto member = _partyTeams.selectMemberById(idMember);
        if (!member) {
            SPDLOG_ERROR("Trying to forward a message to team member {} whom doesn't exist", idMember);
            return;
        }
        // todo add in pending action list
    }

    void FightingPit::addAuthenticatedUser(std::string userName, std::string userToken) {
        _authenticatedPlayers.push_back({ std::move(userName), std::move(userToken) });
    }

    bool FightingPit::isPlayerParticipant(const std::string &name, const std::string &token) const {
        return std::any_of(_authenticatedPlayers.begin(), _authenticatedPlayers.end(),
                           [&name, &token](auto & authPlayer) { return authPlayer.name == name && authPlayer.token == token; } );
    }

    void FightingPit::addPartyTeam(std::unique_ptr<PartyTeam> pt) {
        _partyTeams.addPartyTeam(std::move(pt));
        std::sort(_sideBattles.begin(), _sideBattles.end(),
                [this](auto & lhs, auto & rhs) {
                    return _layoutMapping.activeCharactersOnSide(lhs.getSide()) < _layoutMapping.activeCharactersOnSide(rhs.getSide());
                }
        );
    }

    // Initialization methods used by FightingPitAnnouncer

    void FightingPit::initializeSideBattles() {
        _sideBattles.reserve(HexagonSide::SIDE_NUMBER);

        // Hexagon A creation
        _sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::A_N);
        _sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::A_NE);
        _sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::A_NW);
        _sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::A_S);
        _sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::A_SE);
        _sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::A_SW);

        // Hexagon B creation
        _sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::B_N);
        _sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::B_NE);
        _sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::B_NW);
        _sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::B_S);
        _sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::B_SE);
        _sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::B_SW);

        // Hexagon C creation
        _sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::C_N);
        _sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::C_NE);
        _sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::C_NW);
        _sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::C_S);
        _sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::C_SE);
        _sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::C_SW);

        initializePriorityListInSidesBattle();
    }

    void FightingPit::initializePriorityListInSidesBattle() {
        for (auto & sb : _sideBattles) {
            const auto & memberBySide = _partyTeams.getMembersBySide(sb.getSide());
            const auto & contenderBySide = _contenders.getContenderOnSide(sb.getSide());

            for (const TeamMemberSPtr & member : memberBySide) {
                sb.addParticipantInList(member->getId(), member->getStatus().initialSpeed, false);
            }
            for (unsigned i = 0; i < contenderBySide.size(); ++i) {
                sb.addParticipantInList(i, contenderBySide.at(i)->getStatus().initialSpeed, true);
            }
        };
    }
}
