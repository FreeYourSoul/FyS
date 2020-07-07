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
pit_contenders::getContenderOnSide(hexagon_side::orientation side) const
{
	std::vector<std::shared_ptr<fighting_contender>> result;
	std::copy_if(_contenders.begin(), _contenders.end(), std::back_inserter(result), [side](const auto& contenderPtr) {
		return contenderPtr->getHexagonSideOrient() == side;
	});
	return result;
}

std::vector<std::shared_ptr<fighting_contender>>
pit_contenders::getDeadContenderOnSide(hexagon_side::orientation side) const
{
	std::vector<std::shared_ptr<fighting_contender>> result;
	std::copy_if(_contenders.begin(), _contenders.end(), std::back_inserter(result), [side](const auto& contenderPtr) {
		return contenderPtr->getHexagonSideOrient() == side && contenderPtr->getStatus().life.isDead();
	});
	return result;
}

std::shared_ptr<fighting_contender>
pit_contenders::selectSuitableContenderOnSide(hexagon_side::orientation side, comparator_selection<fighting_contender> comp) const
{
	std::vector<std::shared_ptr<fighting_contender>> result;
	auto it = fys::find_most_suitable(_contenders.begin(), _contenders.end(), [side, &comp](auto& current, auto& next) {
		return current->getHexagonSideOrient() == side && comp(current, next);
	});
	if (it == _contenders.end())
		return nullptr;
	return *it;
}

std::shared_ptr<fighting_contender>
pit_contenders::selectRandomContenderOnSideAlive(hexagon_side::orientation side) const
{
	auto contenderOnSide = getContenderOnSide(side);
	if (contenderOnSide.empty())
		return nullptr;
	uint randomIndex = fys::util::random_generator::generate_in_range(1ul, contenderOnSide.size());
	return contenderOnSide.at(randomIndex - 1);
}

std::shared_ptr<fighting_contender>
pit_contenders::selectSuitableContenderOnSideAlive(hexagon_side::orientation side, comparator_selection<fighting_contender> comp) const
{
	std::vector<std::shared_ptr<fighting_contender>> result;
	auto it = fys::find_most_suitable(_contenders.begin(), _contenders.end(), [side, &comp](auto& current, auto& next) {
		return !current->accessStatus().life.isDead() && !next->accessStatus().life.isDead() &&
				current->getHexagonSideOrient() == side && comp(current, next);
	});
	if (it == _contenders.end())
		return nullptr;
	return *it;
}

std::shared_ptr<fighting_contender>
pit_contenders::selectSuitableContender(comparator_selection<fighting_contender> comp) const
{
	std::vector<std::shared_ptr<fighting_contender>> result;
	auto it = fys::find_most_suitable(_contenders.begin(), _contenders.end(), comp);
	if (it == _contenders.end())
		return nullptr;
	return *it;
}

std::shared_ptr<fighting_contender>
pit_contenders::selectSuitableContenderAlive(comparator_selection<fighting_contender> comp) const
{
	std::vector<std::shared_ptr<fighting_contender>> result;
	auto it = fys::find_most_suitable(_contenders.begin(), _contenders.end(), [&comp](auto& current, auto& next) {
		return !current->accessStatus().life.isDead() && comp(current, next);
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
	const bool isTurnExecuted = c->access_status().process_alteration_before_turn();
	if (isTurnExecuted) {
		c->execute_action();
	}
	c->access_status().process_alteration_after_turn();
}

bool
pit_contenders::addContender(const std::shared_ptr<fighting_contender>& contender)
{
	_contenders.emplace_back(contender);
	_changeSideFlags.emplace_back(false);
	return contender->setup_contender();
}

bool
pit_contenders::allDead() const
{
	return std::all_of(_contenders.begin(), _contenders.end(), [](const auto& contender) {
		return contender->getStatus().life.isDead();
	});
}

unsigned
pit_contenders::contenderOnSide(hexagon_side::orientation side) const
{
	return std::count_if(_contenders.cbegin(), _contenders.cend(),
			[side](const auto& contender) {
				return side == contender->getHexagonSideOrient();
			}
	);
}

}
