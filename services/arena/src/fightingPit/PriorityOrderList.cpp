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
            _priorityList = _baseSpeed;
            _analyzedList = _priorityList;
        }
        else {
            calculatePriority(_analyzedList, _currentTurn);
        }
    }

    void PriorityOrderList::calculatePriority(std::vector<data::PriorityElem> &analyzedList, uint turn) {
        if (_currentTurn != turn || analyzedList.empty()) {
            // log recalculation of priority ?
            return;
        }
        auto &fastest = analyzedList.back();
        if (isPlayerSlowest(fastest.id))
            ++_currentTurn;
        if (analyzedList.size() >= 2)
            fastest.speed -= analyzedList.at(analyzedList.size() - 2).speed;
        _priorityList.emplace_back(fastest);
        if (fastest.speed > 0)
            std::sort(analyzedList.begin(), analyzedList.end());
        else
            analyzedList.pop_back();
        calculatePriority(analyzedList, _currentTurn);
    }

}