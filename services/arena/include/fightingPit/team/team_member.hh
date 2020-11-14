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

#ifndef FYS_TEAMMEMBER_HH
#define FYS_TEAMMEMBER_HH

#include <memory>
#include <variant>

#include <fightingPit/data/common_types.hh>
#include <fightingPit/fighting_pit_layout.hh>
#include <fightingPit/hexagon_side.hh>
#include <sized_queue.hh>

// forward declarations
namespace chaiscript {
class ChaiScript;
}
namespace fys::arena {
class ally_party_teams;
class pit_contenders;
}// namespace fys::arena

namespace fys::arena {

struct contender_target_id {
  uint v;
};
struct contenders_targets_ids {
  std::vector<uint> v;
};
struct ally_target_id {
  uint v;
};
struct allies_targets_ids {
  std::vector<uint> v;
};

/**
 *
 * - the target, optional as every action doesn't require target.
 *   Can be a specific id (of an ally or a contender depending on the action)
 *   Can be a side (as some action can target a whole side)
 */
using target_type = std::variant<
    contender_target_id,
    contenders_targets_ids,
    ally_target_id,
    allies_targets_ids,
    hexagon_side::orientation>;
/**
 * Pending actions of a team member are defined by
 * - the id of the action (mapped as index to the vector TeamMember::_actionDoable)
 * - a target type defined in the alias fys::arena::TargetType
 */
struct pending_action {
  uint id_action{};
  std::optional<target_type> target;
};

/**
 * @brief One entity of a team managed by a client
 */
class team_member {
  friend class fighting_pit_layout;

public:
  team_member(std::string user_name, std::string team_member_name)
      : _user_name(std::move(user_name)), _name(std::move(team_member_name)) {}

  [[nodiscard]] bool
  execute_action(ally_party_teams& apt, pit_contenders& pc, std::unique_ptr<chaiscript::ChaiScript>& chai_ptr);

  void add_pending_action(const std::string& action_name, std::optional<target_type> target);

  /**
   * @brief called by #fys::arena::ally_party_team to set the id of the team member
   */
  void set_id(unsigned id) { _id = id; }
  void add_doable_action(std::string doable, uint level) { _actions_doable.emplace_back(std::move(doable), level); }

  [[nodiscard]] hexagon_side::orientation
  side_orient() const { return (*_side).second; }

  [[nodiscard]] data::status&
  access_status() { return _status; }

  [[nodiscard]] const hexagon_side&
  side() const { return _side; }

  [[nodiscard]] const data::status&
  status() const { return _status; }

  [[nodiscard]] const std::string&
  user_name() const { return _user_name; }

  [[nodiscard]] const std::string&
  name() const { return _name; }

  [[nodiscard]] const std::vector<std::pair<std::string, uint>>&
  actions_doable() const { return _actions_doable; }

  [[nodiscard]] unsigned
  id() const { return _id; }

private:
  std::string _user_name;
  std::string _name;
  hexagon_side _side;
  unsigned _id{};
  data::status _status;

  /**
   * When moving from one side to another the appropriate flag is set in FightingPitLayout which
   * use this member to set the new side position of the character
   */
  hexagon_side::orientation _moving = hexagon_side::orientation::NONE;

  //! action key with level of action
  std::vector<std::pair<std::string, uint>> _actions_doable;
  fys::common::sized_queue<pending_action> _pending_actions;
};

using team_member_sptr = std::shared_ptr<team_member>;

}// namespace fys::arena

#endif//FYS_TEAMMEMBER_HH
