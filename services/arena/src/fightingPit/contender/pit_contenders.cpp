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
#include <functional>

#include <random_generator.hh>
#include <algorithm/algorithm.hh>

#include <fightingPit/contender/fighting_contender.hh>
#include <fightingPit/contender/pit_contenders.hh>
#include <fightingPit/team/ally_party_teams.hh>

namespace fys::arena {

std::vector<std::shared_ptr<fighting_contender>>
pit_contenders::getChangingSideContenders() const
{
	std::vector<std::shared_ptr<fighting_contender>> result;
	result.reserve(_changeSideFlags.size());
	for (std::size_t i = 0; i < _changeSideFlags.size(); ++i) {
		if (_changeSideFlags.at(i)) {
			result.emplace_back(_contenders.at(i));
		}
	}
	return result;
}

std::vector<std::shared_ptr<fighting_contender>>
pit_contenders::get_contender_on_side(hexagon_side::orientation side) const
{
	std::vector<std::shared_ptr<fighting_contender>> result;
	std::copy_if(_contenders.begin(), _contenders.end(), std::back_inserter(result), [side](const auto& contender_ptr) {
		return contender_ptr->get_hexagon_side_orient() == side;
	});
	return result;
}

std::vector<std::shared_ptr<fighting_contender>>
pit_contenders::get_dead_contender_on_side(hexagon_side::orientation side) const
{
	std::vector<std::shared_ptr<fighting_contender>> result;
	std::copy_if(_contenders.begin(), _contenders.end(), std::back_inserter(result), [side](const auto& contender_ptr) {
		return contender_ptr->get_hexagon_side_orient() == side && contender_ptr->get_status().life_pt.is_dead();
	});
	return result;
}

std::shared_ptr<fighting_contender>
pit_contenders::select_suitable_contender_on_side(hexagon_side::orientation side, comparator_selection<fighting_contender> comp) const
{
	std::vector<std::shared_ptr<fighting_contender>> result;
	auto it = fys::find_most_suitable(_contenders.begin(), _contenders.end(), [side, &comp](auto& current, auto& next) {
		return current->get_hexagon_side_orient() == side && comp(current, next);
	});
	if (it == _contenders.end())
		return nullptr;
	return *it;
}

std::shared_ptr<fighting_contender>
pit_contenders::select_random_contender_on_side_alive(hexagon_side::orientation side) const
{
	auto contender_on_side = get_contender_on_side(side);
	if (contender_on_side.empty())
		return nullptr;
	uint randomIndex = fys::util::random_generator::generate_in_range(1ul, contender_on_side.size());
	return contender_on_side.at(randomIndex - 1);
}

std::shared_ptr<fighting_contender>
pit_contenders::select_suitable_contender_on_side_alive(hexagon_side::orientation side, comparator_selection<fighting_contender> comp) const
{
	std::vector<std::shared_ptr<fighting_contender>> result;
	auto it = fys::find_most_suitable(_contenders.begin(), _contenders.end(), [side, &comp](auto& current, auto& next) {
		return !current->access_status().life_pt.is_dead() && !next->access_status().life_pt.is_dead() &&
				current->get_hexagon_side_orient() == side && comp(current, next);
	});
	if (it == _contenders.end())
		return nullptr;
	return *it;
}

std::shared_ptr<fighting_contender>
pit_contenders::select_suitable_contender(comparator_selection<fighting_contender> comp) const
{
	std::vector<std::shared_ptr<fighting_contender>> result;
	auto it = fys::find_most_suitable(_contenders.begin(), _contenders.end(), comp);
	if (it == _contenders.end())
		return nullptr;
	return *it;
}

std::shared_ptr<fighting_contender>
pit_contenders::select_suitable_contender_alive(comparator_selection<fighting_contender> comp) const
{
	std::vector<std::shared_ptr<fighting_contender>> result;
	auto it = fys::find_most_suitable(_contenders.begin(), _contenders.end(), [&comp](auto& current, auto& next) {
		return !current->access_status().life_pt.is_dead() && comp(current, next);
	});
	if (it == _contenders.end())
		return nullptr;
	return *it;
}

void
pit_contenders::execute_contender_action(const data::priority_elem& contender)
{
	if (!contender.is_contender || contender.id >= _contenders.size()) {
		return;
	}
	auto& c = _contenders.at(contender.id);
	const bool is_turn_executed = c->access_status().process_alteration_before_turn();
	if (is_turn_executed) {
		c->execute_action();
	}
	c->access_status().process_alteration_after_turn();
}

bool
pit_contenders::add_contender(const std::shared_ptr<fighting_contender>& contender)
{
	_contenders.emplace_back(contender);
	_changeSideFlags.emplace_back(false);
	return contender->setup_contender();
}

bool
pit_contenders::all_dead() const
{
	return std::all_of(_contenders.begin(), _contenders.end(), [](const auto& contender) {
		return contender->get_status().life_pt.is_dead();
	});
}

unsigned
pit_contenders::contender_on_side(hexagon_side::orientation side) const
{
	return std::count_if(_contenders.cbegin(), _contenders.cend(),
			[side](const auto& contender) {
				return side == contender->get_hexagon_side_orient();
			}
	);
}

}
