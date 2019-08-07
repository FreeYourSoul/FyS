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

#include <fightingPit/contender/FightingContender.hh>
#include <fightingPit/contender/PitContenders.hh>

namespace fys::arena {

    std::vector<std::shared_ptr<FightingContender>> PitContenders::getChangingSideContenders() {
        std::vector<std::shared_ptr<FightingContender>> result;
        result.reserve(_changeSideFlags.size());
        for (std::size_t i = 0; i < _changeSideFlags.size(); ++i) {
            if (_changeSideFlags.at(i)) {
                result.emplace_back(_contenders.at(i));
            }
        }
        return result;
    }

    std::vector<std::shared_ptr<FightingContender>> PitContenders::getContenderOnSide(fys::arena::HexagonSide::Orientation side) {
        std::vector<std::shared_ptr<FightingContender>> result;
        std::copy_if(_contenders.begin(), _contenders.end(), result.begin(), [side](const auto &contenderPtr){
            return (*contenderPtr->getHexagonSide()).second == side;
        });
        return result;
    }

    void PitContenders::executeContenderAction(const data::PriorityElem &contender, PitContenders &pitContenders, AllyPartyTeams &AllyPartyTeams) {
        if (!contender.isContender && contender.id < _contenders.size()) {
            // TODO log error on execute
            return;
        }
        _contenders.at(contender.id)->executeAction(pitContenders, AllyPartyTeams);
    }

}
