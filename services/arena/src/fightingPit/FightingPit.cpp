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
#include <chrono>
#include <fightingPit/FightingPit.hh>

namespace {
    std::chrono::milliseconds retrieveTimeInterludeFromLevelDegree(fys::arena::FightingPit::Level level) {
        switch () {
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

    void FightingPit::startBattle() {
        std::chrono::milliseconds timerInterlude = retrieveTimeInterludeFromLevelDegree(_levelFightingPit);
        while (_end == Ending::NOT_FINISHED) {
            // TODO take player inputs
            //          if player inputs, add pending action to character (override the pending action in case one already there)
            readInputAndAppendPendingActions();
            auto now = std::chrono::system_clock::now();
            for (auto &side : _sideBattles) {
                auto &currentParticipant = side->getCurrentParticipantTurn(now, timerInterlude);

                if (currentParticipant->isContender) {
                    // If non-playable character (ennemy NPC)
                    // TODO execute monster script action
                } else {
                    // If character of a player
                    // TODO check if character has a handling action and execute it
                }
            }
        }
    }

    void FightingPit::readInputAndAppendPendingActions() {

    }


}
