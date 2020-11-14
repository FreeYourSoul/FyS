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

#include <fightingPit/data/common_types.hh>
#include <utility>
#include <vector>

namespace fys::arena {

class priority_order_list {

public:
  priority_order_list() = default;
  explicit priority_order_list(std::vector<data::priority_elem> base_speed);

  void add_participant_in_list(std::uint32_t prio_elem, int speed, bool is_contender);
  void rm_participant_from_list(std::uint32_t id_participant, bool is_contender);

  [[nodiscard]] bool
  empty() const { return _base_speed.empty(); }

  [[nodiscard]] const data::priority_elem&
  get_current() const noexcept { return _current_prio; }

  [[nodiscard]] unsigned
  get_turn_number() const noexcept { return !_current_turn ? 1 : _current_turn; }

  data::priority_elem
  next();

private:
  void sort_base_and_calculate_priority();

  void calculate_priority(std::uint32_t turn);

  void end_turn_routine();

  [[nodiscard]] int
  get_computed_speed(const data::priority_elem& analyst_elem) const;

  /**
   * Sort in a specific manner
   */
  void custom_sort();

  [[nodiscard]] bool
  is_player_slowest(std::uint32_t id) const {
    return _base_speed.front().id == id;
  }

  [[nodiscard]] int
  get_fastest_base_speed() const {
    return _base_speed.back().speed;
  }

  [[nodiscard]] int
  get_slowest_speed() const {
    return _base_speed.front().speed;
  }

private:
  std::vector<data::priority_elem> _base_speed;
  std::vector<data::priority_elem> _priority_list;
  // List used as temporary for the calculate priority
  std::vector<data::priority_elem> _analyzed_list;

  data::priority_elem _current_prio{};

  std::uint32_t _current_turn = 0;
};

}// namespace fys::arena

#endif//FYS_PRIORITYORDERLIST_HH
