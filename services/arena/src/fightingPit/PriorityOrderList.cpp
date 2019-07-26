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

#include <algorithm>
#include <fightingPit/PriorityOrderList.hh>

namespace fys::arena {
    
    PriorityOrderList::PriorityOrderList(std::vector<data::PriorityElem> baseSpeed) : 
                        _baseSpeed(std::move(baseSpeed)), _priorityList(_baseSpeed.size() * 4) 
    {
        sortBaseAndCalculatePriority();
    }

    void PriorityOrderList::addParticipantInList(uint id, int speed, bool isContender) {
        _baseSpeed.emplace_back(id, speed, isContender);
        sortBaseAndCalculatePriority();
    }

    void PriorityOrderList::removeParticipantFromList(uint idParticipant) {
        auto findParticipantPredicate = [idParticipant](const data::PriorityElem& elem) { return elem.id == idParticipant; };
        _baseSpeed.erase(std::remove_if(_baseSpeed.begin(), _baseSpeed.end(), findParticipantPredicate), _priorityList.end());
        _priorityList.erase(std::remove_if(_priorityList.begin(), _priorityList.end(), findParticipantPredicate), _priorityList.end());
        _analyzedList.erase(std::remove_if(_analyzedList.begin(), _analyzedList.end(), findParticipantPredicate), _analyzedList.end());
        sortBaseAndCalculatePriority();
    }

    data::PriorityElem PriorityOrderList::getNext() {

    }

    void PriorityOrderList::sortBaseAndCalculatePriority() {
        std::sort(_baseSpeed.begin(), _baseSpeed.end());
        if (_currentTurn == 0) {
            _analyzedList = _baseSpeed;
        }
        calculatePriority(_analyzedList, _currentTurn);
    }

    int PriorityOrderList::getComputedSpeed(const data::PriorityElem &elemToCompute) const {
        for (std::size_t i = 0; i < _baseSpeed.size(); ++i) {
            if (_baseSpeed.at(i).id == elemToCompute.id) {
                uint idNextInLine = (i + 1 < _baseSpeed.size()) ? _baseSpeed.front().id : _baseSpeed.at(i + 1).id;
                for (const auto &prioElemInList : _priorityList) {
                    if (prioElemInList.id == idNextInLine) {
                        return prioElemInList.speed;
                    }
                }
            }
        }
        // TODO : Log warning about strange stuff happenning arround here
        return 0;
    }

    void ProrityOrderList::enTurnRoutine() {
        ++_currentTurn;
        if (_baseSpeed.size() <= 1)
            return;
        for (const auto &baseSpeedElem : _baseSpeed) {
            for (std::size_t i = 0; i < _analyzedList.size(); ++i) {
                if (_analyzedList.at(i).id == baseSpeedElem.id) {
                    _analyzedList.at(i).speed += baseSpeedElem.speed + getFastestBaseSpeed();
                    break;
                }
            }
        }
    }

    void PriorityOrderList::calculatePriority(std::vector<data::PriorityElem> &analyzedList, uint turn) {
        if (_currentTurn != turn || analyzedList.empty()) {
            // log recalculation of priority ?
            return;
        }
        std::sort(analyzedList.begin(), analyzedList.end());
        auto &fastest = analyzedList.back();

        if (analyzedList.size() >= 2)
            fastest.speed = getComputedSpeed(fastest);
        _priorityList.emplace_back(fastest);
        if (isPlayerSlowest(fastest.id))
            endTurnRoutine();
        calculatePriority(analyzedList, turn);
    }

}
