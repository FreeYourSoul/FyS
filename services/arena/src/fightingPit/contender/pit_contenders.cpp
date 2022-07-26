// MIT License
//
// Copyright (c) 2021-2022 Quentin Balland
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

#include <functional>
#include <spdlog/spdlog.h>

#include <random_generator.hh>

#include <fil/algorithm/suitable.hh>

#include <fightingPit/contender/fighting_contender.hh>
#include <fightingPit/contender/pit_contenders.hh>
#include <fightingPit/team/ally_party_teams.hh>

namespace fys::arena {

std::vector<std::shared_ptr<fighting_contender>>
pit_contenders::changing_side_contenders() const {
  std::vector<std::shared_ptr<fighting_contender>> result;
  result.reserve(_change_side_flags.size());
  for (std::size_t i = 0; i < _change_side_flags.size(); ++i) {
    if (_change_side_flags.at(i)) {
      result.emplace_back(_contenders.at(i));
    }
  }
  return result;
}

std::vector<std::shared_ptr<fighting_contender>>
pit_contenders::contenders_on_side(hexagon_side::orientation side) const {
  std::vector<std::shared_ptr<fighting_contender>> result;
  std::ranges::copy_if(_contenders, std::back_inserter(result), [side](const auto& contender_ptr) {
    return contender_ptr->side_orient() == side;
  });
  return result;
}

std::vector<std::shared_ptr<fighting_contender>>
pit_contenders::get_dead_contender_on_side(hexagon_side::orientation side) const {
  std::vector<std::shared_ptr<fighting_contender>> result;
  std::ranges::copy_if(_contenders, std::back_inserter(result), [side](const auto& contender_ptr) {
    return contender_ptr->side_orient() == side && contender_ptr->status().life_pt.is_dead();
  });
  return result;
}

std::shared_ptr<fighting_contender>
pit_contenders::select_suitable_contender_on_side(hexagon_side::orientation side, comparator_selection<fighting_contender> comp) const {
  std::vector<std::shared_ptr<fighting_contender>> result;
  auto it = fil::find_most_suitable(_contenders.begin(), _contenders.end(), [side, &comp](auto& current, auto& next) {
    return current->side_orient() == side && comp(current, next);
  });
  if (it == _contenders.end())
    return nullptr;
  return *it;
}

std::shared_ptr<fighting_contender>
pit_contenders::select_random_contender_on_side_alive(hexagon_side::orientation side) const {
  auto contenders = contenders_on_side(side);
  if (contenders.empty()) {
    return nullptr;
  }
  std::uint32_t randomIndex = fys::util::random_generator::generate_in_range(1ul, contenders.size());
  return contenders.at(randomIndex - 1);
}

std::shared_ptr<fighting_contender>
pit_contenders::select_suitable_contender(comparator_selection<fighting_contender> comp) const {
  std::vector<std::shared_ptr<fighting_contender>> result;
  auto it = fil::find_most_suitable(_contenders.begin(), _contenders.end(), comp);
  if (it == _contenders.end()) {
    return nullptr;
  }
  return *it;
}

std::shared_ptr<fighting_contender>
pit_contenders::select_suitable_contender_on_side_alive(hexagon_side::orientation side, comparator_selection<fighting_contender> comp) const {
  std::vector<std::shared_ptr<fighting_contender>> result;
  auto it = fil::find_most_suitable(_contenders.begin(), _contenders.end(), [side, &comp](auto& current, auto& next) {
    return !current->access_status().life_pt.is_dead() && !next->access_status().life_pt.is_dead() && current->side_orient() == side && comp(current, next);
  });
  if (it == _contenders.end()) {
    return nullptr;
  }
  return *it;
}

std::shared_ptr<fighting_contender>
pit_contenders::select_suitable_contender_alive(comparator_selection<fighting_contender> comp) const {
  std::vector<std::shared_ptr<fighting_contender>> result;
  auto it = fil::find_most_suitable(_contenders.begin(), _contenders.end(), [&comp](auto& current, auto& next) {
    return !current->access_status().life_pt.is_dead() && comp(current, next);
  });
  if (it == _contenders.end())
    return nullptr;
  return *it;
}

void pit_contenders::execute_contender_action(const data::priority_elem& contender) {
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

bool pit_contenders::add_contender(const std::shared_ptr<fighting_contender>& contender) {
  _contenders.emplace_back(contender);
  _change_side_flags.emplace_back(false);
  return contender->setup_contender();
}

bool pit_contenders::all_dead() const {
  return std::ranges::all_of(_contenders, [](const auto& contender) { return contender->status().life_pt.is_dead(); });
}

unsigned
pit_contenders::number_contender_on_side(hexagon_side::orientation side) const {
  return std::ranges::count_if(_contenders, [side](const auto& contender) { return side == contender->side_orient(); });
}

}// namespace fys::arena
