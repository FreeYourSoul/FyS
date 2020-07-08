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
#include <fightingPit/priority_order_list.hh>

namespace fys::arena {

priority_order_list::priority_order_list(std::vector<data::priority_elem> base_speed)
		:
		_base_speed(std::move(base_speed))
{
	_priorityList.reserve(_base_speed.size() * 4);
	uint turn = _current_turn;
	sort_base_and_calculate_priority();
	_current_turn = turn;

}

void
priority_order_list::add_participant_in_list(uint id, int speed, bool is_contender)
{
	if (std::any_of(_base_speed.begin(), _base_speed.end(),
			[id, is_contender](const auto& prio_elem) { return prio_elem.id == id && prio_elem.is_contender == is_contender; })) {
		SPDLOG_WARN("Cannot add participant in order list : Participant of id {} already existing", id);
		return;
	}
	_base_speed.emplace_back(id, speed, is_contender);
	if (_current_turn > 0)
		speed += get_fastest_base_speed();
	_analyzed_list.emplace_back(id, speed, is_contender);
	_priorityList.clear();
	uint turn = _current_turn;
	sort_base_and_calculate_priority();
	_current_turn = turn;
}

void
priority_order_list::rm_participant_from_list(uint id_participant, bool is_contender)
{
	auto findParticipantPredicate = [id_participant, is_contender](const data::priority_elem& elem) {
		return elem.id == id_participant && elem.is_contender == is_contender;
	};
	_base_speed.erase(std::remove_if(_base_speed.begin(), _base_speed.end(), findParticipantPredicate), _base_speed.end());
	_priorityList.erase(std::remove_if(_priorityList.begin(), _priorityList.end(), findParticipantPredicate), _priorityList.end());
	_analyzed_list.erase(std::remove_if(_analyzed_list.begin(), _analyzed_list.end(), findParticipantPredicate), _analyzed_list.end());
}

data::priority_elem
priority_order_list::get_next()
{
	if (_base_speed.empty()) {
		return {0, 0, false};
	}
	if (_priorityList.empty()) {
		// Turn increase automatically if it is the first turn
		if (_current_turn == 0) {
			++_current_turn;
		}
		sort_base_and_calculate_priority();
		return get_next();
	}
	_current_prio = _priorityList.back();
	_priorityList.pop_back();
	return _current_prio;
}

void
priority_order_list::custom_sort()
{
	std::sort(_analyzed_list.begin(), _analyzed_list.end());

	// reorder equal speeds
	if (auto found = std::adjacent_find(_analyzed_list.begin(), _analyzed_list.end(),
				[this](const auto& e, const auto& e2) {
					if (e.speed == e2.speed) {
						uint baseSpeedE = 0;
						uint baseSpeedE2 = 0;
						for (const auto& baseSpeedElem : _base_speed) {
							if (baseSpeedElem.id == e.id)
								baseSpeedE = baseSpeedElem.speed;
							if (baseSpeedElem.id == e2.id)
								baseSpeedE2 = baseSpeedElem.speed;
						}
						return baseSpeedE > baseSpeedE2;
					}
					return false;
				});
			found != _analyzed_list.end()) {
		if (auto toSwap = std::adjacent_find(_analyzed_list.begin(), _analyzed_list.end(),
					[](const auto& e, const auto& e2) { return e.speed != e2.speed; });
				toSwap != _analyzed_list.end()) {
			std::reverse(found, toSwap + 1);
		}
	}
}

void
priority_order_list::sort_base_and_calculate_priority()
{
	if (_base_speed.empty())
		return;
	std::sort(_base_speed.begin(), _base_speed.end());
	if (_current_turn == 0) {
		_analyzed_list = _base_speed;
		_priorityList = _base_speed;
		++_current_turn;
		return;
	}
	calculate_priority(_current_turn);
}

int
priority_order_list::get_computed_speed(const data::priority_elem& elem_to_compute) const
{
	for (std::size_t i = 0; i < _base_speed.size(); ++i) {
		if (_base_speed.at(i).id == elem_to_compute.id) {
			uint idNextInLine = (i == 0) ? _base_speed.back().id : _base_speed.at(i - 1).id;
			if (auto it = std::find_if(_analyzed_list.begin(), _analyzed_list.end(),
						[idNextInLine](const auto& analistElem) {
							return analistElem.id == idNextInLine;
						}); it != _analyzed_list.end()) {
				return it->speed ? it->speed : 1;
			}
		}
	}
	return 1;
}

void
priority_order_list::end_turn_routine()
{
	++_current_turn;
	if (_base_speed.size() <= 1)
		return;

	for (const auto& baseSpeedElem : _base_speed) {
		if (auto it = std::find_if(_analyzed_list.begin(), _analyzed_list.end(),
					[baseId = baseSpeedElem.id](const auto& analyzedElem) {
						return analyzedElem.id == baseId;
					});
				it != _analyzed_list.end()) {
			it->speed += baseSpeedElem.speed + get_fastest_base_speed();
		}
	}
	// reverse in order to have faster at the end (to use pop_back)
	custom_sort();
	std::reverse(_priorityList.begin(), _priorityList.end());
}

void
priority_order_list::calculate_priority(uint turn)
{
	if (_current_turn != turn) {
		// log recalculation of priority ?
		return;
	}
	custom_sort();

	auto& fastest = _analyzed_list.back();

	if (_analyzed_list.size() > 1)
		fastest.speed -= get_computed_speed(fastest);
	_priorityList.emplace_back(fastest);
	if (is_player_slowest(fastest.id)) {
		end_turn_routine();
	}
	calculate_priority(turn);
}

}
