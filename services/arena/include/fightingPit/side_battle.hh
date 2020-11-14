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

#ifndef FYS_SIDEBATTLE_HH
#define FYS_SIDEBATTLE_HH

#include <chrono>
#include <functional>

#include <fightingPit/hexagon_side.hh>
#include <fightingPit/priority_order_list.hh>

//forward declaration
namespace fys::arena {
class pit_contenders;
class ally_party_teams;
class team_member;
class fighting_contender;
}// namespace fys::arena
// end forward declaration

namespace fys::arena {

/**
 * @brief Represent a battle on a specific side of one of the 3 hexagons
 *
 */
class side_battle {

public:
  side_battle(pit_contenders& pit_contenders, ally_party_teams& ally_party_teams, hexagon_side::orientation side)
      : _contenders(pit_contenders),
        _partyTeams(ally_party_teams),
        _side(side),
        _turnDone(true) {}

  /**
   * Get the next participant in the fighting pit turn (can be either an NPC, or a contender) depending on their
   * speed. This Calculation is made in the fys::arena::PriorityOrderList.
   *
   * @param now timing now in milliseconds (timestamp)
   * @param timer_interlude timing of a turn for a player (depend on the level set by the creator of the arena)
   * @return PriorityElem representing the next player to play its turn
   */
  [[nodiscard]] data::priority_elem
  get_current_participant_turn(const std::chrono::system_clock::time_point& now,
                               const std::chrono::milliseconds& timer_interlude);

  /**
   * Erase characters (contender or ally) from the PriorityOrderList that are dead.
   */
  void erase_finished_alteration_and_dead_characters_from_turn_list();

  void rm_participant_from_list(const team_member& tm);
  void rm_participant_from_list(const fighting_contender& contender);
  void add_participant_in_list(const team_member& tm);
  void add_participant_in_list(const fighting_contender& contender);
  void add_participant_in_list(uint id, int speed, bool isContender) {
    _priority_order_list.add_participant_in_list(id, speed, isContender);
  }
  void turn_done(bool turnDone) { _turnDone = turnDone; };

  [[nodiscard]] hexagon_side::orientation
  side() const { return _side; }

  [[nodiscard]] bool
  empty() const { return _priority_order_list.empty(); }

private:
  std::reference_wrapper<pit_contenders> _contenders;
  std::reference_wrapper<ally_party_teams> _partyTeams;

  priority_order_list _priority_order_list;
  hexagon_side::orientation _side;

  bool _turnDone;
  std::chrono::system_clock::time_point _endCurrentTurn;
};

}// namespace fys::arena

#endif//FYS_SIDEBATTLE_HH
