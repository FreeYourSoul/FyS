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

#ifndef FYS_FIGHTINGPITLAYOUT_HH
#define FYS_FIGHTINGPITLAYOUT_HH

#include <functional>
#include <map>

#include <fightingPit/hexagon_side.hh>

// forward declaration
namespace fys::arena {
class side_battle;
class ally_party_teams;
class pit_contenders;
class party_team;
class team_member;
class fighting_contender;
}// namespace fys::arena
// end forward declaration

namespace fys::arena {

/**
 * @brief Layout of the fighting pit. Manage the positions of monsters and the players/monsters movements
 *
 * This class is mapping the characters (contenders or NPC) with the layout of the fightingpit.
 * The layout of the fighting pit can be taken as a draughtboard composed of 3 hexagons as shown as below
 *                        _____
 *                       /     \
 *                 _____/  B    \        18 possible playable sides
 *                /     \       /        6  possible ambush sides [A_NE, B_SW, A_SE, C_NW, C_N, B_S]
 *               /  A    \_____/
 *               \       /     \
 *                \_____/  C    \
 *                      \       /
 *                       \_____/
 *
 * Each hexagon represent the monster position. The Player can have character of its team spread on the lines representing
 * the sides of the hexagons.
 * Here is the hexagon at coordinate A, with its sides (where the characters can stands to attack the monster).
 *
 *           A_N
 *           _____                N  = North
 * A_NW     /     \    A_NE       NE = North-East
 *         /  A    \              NW = North-West
 *         \       /              S  = South
 * A_SW     \_____/    A_SE       SE = South-East
 *                                SW = South-West
 *           A_S
 *
 */
class fighting_pit_layout {

public:
  fighting_pit_layout(pit_contenders& contenders, ally_party_teams& partyTeams);

  void add_active_party_team(const party_team& pt);
  void add_active_contender(uint number_contender_to_add = 1u);

  /**
   * Retrieve the number of active characters (contenders or players) on the given side
   * @param side to check the contender and/or players
   * @return number of active player on the side
   */
  [[nodiscard]] unsigned
  active_characters_on_side(hexagon_side::orientation side) const;

  bool
  initiate_contender_move(std::shared_ptr<fighting_contender> contender, hexagon_side::orientation move_to);
  bool
  initiate_member_move(std::shared_ptr<team_member> member, hexagon_side::orientation move_to);

  void initiate_force_contender_move(std::shared_ptr<fighting_contender> contender, hexagon_side::orientation move_to);
  void initiate_force_member_move(std::shared_ptr<team_member> member, hexagon_side::orientation move_to);
  void initiate_contender_move_dir(std::shared_ptr<fighting_contender> contender, data::move_direction move_dir);
  void initiate_member_move_dir(std::shared_ptr<team_member> member, data::move_direction move_dir);

  void execute_movements(std::vector<side_battle>& sides);

  static void set_contender_initiate_position(fighting_contender& contender, hexagon_side::orientation side);
  static void set_ally_move_initiate_position(team_member& tm, hexagon_side::orientation side);

private:
  [[nodiscard]] std::vector<std::shared_ptr<team_member>>
  get_changing_side_team_members();

  [[nodiscard]] std::vector<std::shared_ptr<fighting_contender>>
  changing_side_contenders();

private:
  std::reference_wrapper<pit_contenders> _contenders;
  std::reference_wrapper<ally_party_teams> _party_teams;

  //! map of <userName with memberId> on isMoving
  std::map<std::pair<std::string, uint>, bool> _moving_flag_ally;

  // vector of bool react differently from other vectors (memory optimization)
  // but it doesn't matter in this case as we are not in a multi-threaded context
  //! vector of isMoving for contenders (index of the vector is the id of the contender)
  std::vector<bool> _moving_flag_contender;
};

}// namespace fys::arena

#endif// !FYS_FIGHTINGPITLAYOUT_HH
