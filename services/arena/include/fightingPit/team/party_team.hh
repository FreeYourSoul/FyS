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

#ifndef FYS_PARTYTEAM_HH
#define FYS_PARTYTEAM_HH

#include <fightingPit/hexagon_side.hh>
#include <memory>
#include <utility>
#include <vector>

namespace fys::arena {

//forward declarations
class team_member;

class party_team {

public:
  explicit party_team(std::string user_name)
      : _user_name(std::move(user_name)) {}

  [[nodiscard]] std::vector<std::shared_ptr<team_member>>
  team_member_on_side(hexagon_side::orientation orientation) const;

  [[nodiscard]] const std::string&
  user_name() const { return _user_name; }

  [[nodiscard]] const std::vector<std::shared_ptr<team_member>>&
  team_members() const { return _members; }

  [[nodiscard]] std::vector<std::shared_ptr<team_member>>&
  access_team_members() { return _members; }

  [[nodiscard]] unsigned
  ally_number_on_side(hexagon_side::orientation side) const;

  [[nodiscard]] std::vector<std::shared_ptr<team_member>>
  get_dead_team_members_on_side(fys::arena::hexagon_side::orientation tm_ptr) const;

  [[nodiscard]] bool
  is_team_ready() const { return _ready; }

  void set_team_ready(bool is_ready) { _ready = is_ready; }
  void add_team_member(std::shared_ptr<team_member> member);

private:
  std::string _user_name;
  std::vector<std::shared_ptr<team_member>> _members;
  bool _ready = false;
};

using party_team_uptr = std::unique_ptr<party_team>;

}// namespace fys::arena

#endif//FYS_PARTYTEAM_HH
