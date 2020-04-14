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
#include <fightingPit/PriorityOrderList.hh>

namespace fys::arena {

PriorityOrderList::PriorityOrderList(std::vector<data::PriorityElem> baseSpeed)
		:
		_baseSpeed(std::move(baseSpeed))
{
	_priorityList.reserve(_baseSpeed.size() * 4);
	uint turn = _currentTurn;
	sortBaseAndCalculatePriority();
	_currentTurn = turn;

}

void
PriorityOrderList::addParticipantInList(uint id, int speed, bool isContender)
{
	if (std::any_of(_baseSpeed.begin(), _baseSpeed.end(),
			[id, isContender](const auto& prioElem) { return prioElem.id == id && prioElem.isContender == isContender; })) {
		SPDLOG_WARN("Cannot add participant in order list : Participant of id {} already existing", id);
		return;
	}
	_baseSpeed.emplace_back(id, speed, isContender);
	if (_currentTurn > 0)
		speed += getFastestBaseSpeed();
	_analyzedList.emplace_back(id, speed, isContender);
	_priorityList.clear();
	uint turn = _currentTurn;
	sortBaseAndCalculatePriority();
	_currentTurn = turn;
}

void
PriorityOrderList::removeParticipantFromList(uint idParticipant, bool isContender)
{
	auto findParticipantPredicate = [idParticipant, isContender](const data::PriorityElem& elem) {
		return elem.id == idParticipant && elem.isContender == isContender;
	};
	_baseSpeed.erase(std::remove_if(_baseSpeed.begin(), _baseSpeed.end(), findParticipantPredicate), _baseSpeed.end());
	_priorityList.erase(std::remove_if(_priorityList.begin(), _priorityList.end(), findParticipantPredicate), _priorityList.end());
	_analyzedList.erase(std::remove_if(_analyzedList.begin(), _analyzedList.end(), findParticipantPredicate), _analyzedList.end());
}

data::PriorityElem
PriorityOrderList::getNext()
{
	if (_baseSpeed.empty()) {
		return {0, 0, false};
	}
	if (_priorityList.empty()) {
		// Turn increase automatically if it is the first turn
		if (_currentTurn == 0) {
			++_currentTurn;
		}
		sortBaseAndCalculatePriority();
		return getNext();
	}
	_currentPrio = _priorityList.back();
	_priorityList.pop_back();
	return _currentPrio;
}

void
PriorityOrderList::customSort()
{
	std::sort(_analyzedList.begin(), _analyzedList.end());

	// reorder equal speeds
	if (auto found = std::adjacent_find(_analyzedList.begin(), _analyzedList.end(),
				[this](const auto& e, const auto& e2) {
					if (e.speed == e2.speed) {
						uint baseSpeedE = 0;
						uint baseSpeedE2 = 0;
						for (const auto& baseSpeedElem : _baseSpeed) {
							if (baseSpeedElem.id == e.id)
								baseSpeedE = baseSpeedElem.speed;
							if (baseSpeedElem.id == e2.id)
								baseSpeedE2 = baseSpeedElem.speed;
						}
						return baseSpeedE > baseSpeedE2;
					}
					return false;
				});
			found != _analyzedList.end()) {
		if (auto toSwap = std::adjacent_find(_analyzedList.begin(), _analyzedList.end(),
					[](const auto& e, const auto& e2) { return e.speed != e2.speed; });
				toSwap != _analyzedList.end()) {
			std::reverse(found, toSwap + 1);
		}
	}
}

void
PriorityOrderList::sortBaseAndCalculatePriority()
{
	if (_baseSpeed.empty())
		return;
	std::sort(_baseSpeed.begin(), _baseSpeed.end());
	if (_currentTurn == 0) {
		_analyzedList = _baseSpeed;
		_priorityList = _baseSpeed;
		++_currentTurn;
		return;
	}
	calculatePriority(_currentTurn);
}

int
PriorityOrderList::getComputedSpeed(const data::PriorityElem& elemToCompute) const
{
	for (std::size_t i = 0; i < _baseSpeed.size(); ++i) {
		if (_baseSpeed.at(i).id == elemToCompute.id) {
			uint idNextInLine = (i == 0) ? _baseSpeed.back().id : _baseSpeed.at(i - 1).id;
			if (auto it = std::find_if(_analyzedList.begin(), _analyzedList.end(),
						[idNextInLine](const auto& analistElem) {
							return analistElem.id == idNextInLine;
						}); it != _analyzedList.end()) {
				return it->speed ? it->speed : 1;
			}
		}
	}
	// TODO : Log warning about strange stuff happening around here
	return 1;
}

void
PriorityOrderList::endTurnRoutine()
{
	++_currentTurn;
	if (_baseSpeed.size() <= 1)
		return;

	for (const auto& baseSpeedElem : _baseSpeed) {
		if (auto it = std::find_if(_analyzedList.begin(), _analyzedList.end(),
					[baseId = baseSpeedElem.id](const auto& analyzedElem) {
						return analyzedElem.id == baseId;
					});
				it != _analyzedList.end()) {
			it->speed += baseSpeedElem.speed + getFastestBaseSpeed();
		}
	}
	// reverse in order to have faster at the end (to use pop_back)
	customSort();
	std::reverse(_priorityList.begin(), _priorityList.end());
}

void
PriorityOrderList::calculatePriority(uint turn)
{
	if (_currentTurn != turn) {
		// log recalculation of priority ?
		return;
	}
	customSort();

	auto& fastest = _analyzedList.back();

	if (_analyzedList.size() > 1)
		fastest.speed -= getComputedSpeed(fastest);
	_priorityList.emplace_back(fastest);
	if (isPlayerSlowest(fastest.id)) {
		endTurnRoutine();
	}
	calculatePriority(turn);
}

}
