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
#include <HexagonSide>
#include <fightingPit/contender/FightingContender.hh>
#include <fightingPit/FightingPit.hh>

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
                return std::chrono::milliseconds {10000};
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

    void FightingPit::continueBattle(const std::chrono::system_clock::time_point & now) {
        for (auto &side : _sideBattles) {
            auto currentParticipant = side->getCurrentParticipantTurn(now, _timeInterlude);

            if (currentParticipant.isContender) {
                // non-playable character (enemy NPC)
                _contenders.executeContenderAction(currentParticipant);
            } else {
                // character of a player
                // TODO check if character has a handling action and execute it
            }
        }
    }

    void FightingPit::addAuthenticatedUser(std::string userName, std::string userToken) {
        _authenticatedPlayers.push_back({ std::move(userName), std::move(userToken) });
    }

    bool FightingPit::isPlayerParticipant(const std::string &name, const std::string &token) const {
        return std::any_of(_authenticatedPlayers.begin(), _authenticatedPlayers.end(),
                           [&name, &token](auto & authPlayer) { return authPlayer.name == name && authPlayer.token == token; } );
    }

    void FightingPit::forwardMessageToTeamMember(unsigned idMember) {
        auto member = _partyTeams.selectMemberById(idMember);
        if (!member) {
            SPDLOG_ERROR("Trying to forward a message to team member {} whom doesn't exist", idMember);
            return;
        }
        // todo add in pending action list
    }

    void FightingPit::addPartyTeam(std::unique_ptr<PartyTeam> pt) {
        _partyTeams.addPartyTeam(std::move(pt));
        std::sort(_sideBattles.begin(), _sideBattles.end(),
                [this](auto & sideLhs, auto & sideRhs) {
                    return _layoutMapping.activeCharactersOnSide(sideLhs) < _layoutMapping.activeCharactersOnSide(sideRhs);
                }
        );
    }
}
