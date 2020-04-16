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


#ifndef FYS_PRIORITYORDERLIST_HH
#define FYS_PRIORITYORDERLIST_HH

#include <utility>
#include <vector>
#include <fightingPit/data/CommonTypes.hh>

namespace fys::arena {

class PriorityOrderList {

public:
	PriorityOrderList() = default;
	explicit PriorityOrderList(std::vector<data::PriorityElem> baseSpeed);

	void addParticipantInList(uint id, int speed, bool isContender);
	void removeParticipantFromList(uint idParticipant, bool isContender);

	[[nodiscard]] bool
	empty() const { return _baseSpeed.empty(); }

	[[nodiscard]] const data::PriorityElem&
	getCurrent() const noexcept { return _currentPrio; }

	[[nodiscard]] unsigned
	getTurnNumber() const noexcept { return !_currentTurn ? 1 : _currentTurn; }

	data::PriorityElem
	getNext();

private:
	void
	sortBaseAndCalculatePriority();

	void
	calculatePriority(uint turn);

	void
	endTurnRoutine();

	[[nodiscard]] int
	getComputedSpeed(const data::PriorityElem& elemToCompute) const;

	/**
	 * Sort in a specific manner
	 */
	void
	customSort();

	[[nodiscard]] bool
	isPlayerSlowest(uint id) const
	{
		return _baseSpeed.front().id == id;
	}

	[[nodiscard]] int
	getFastestBaseSpeed() const
	{
		return _baseSpeed.back().speed;
	}

	[[nodiscard]] int
	getSlowestSpeed() const
	{
		return _baseSpeed.front().speed;
	}

private:
	std::vector<data::PriorityElem> _baseSpeed;
	std::vector<data::PriorityElem> _priorityList;
	// List used as temporary for the calculate priority
	std::vector<data::PriorityElem> _analyzedList;

	data::PriorityElem _currentPrio{};

	uint _currentTurn = 0;

};

}

#endif //FYS_PRIORITYORDERLIST_HH
