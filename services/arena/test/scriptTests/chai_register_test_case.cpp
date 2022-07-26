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

#include <catch2/catch.hpp>
#include <map>

#include <chaiscript/chaiscript.hpp>

#include <fightingPit/contender/pit_contenders.hh>
#include <fightingPit/team/ally_party_teams.hh>
#include <fightingPit/team/party_team.hh>
#include <fightingPit/team/team_member.hh>

#include <CmlCopy.hh>

#include <chai_register.hh>

namespace {
std::string
local_path_storage() {
  return (std::filesystem::temp_directory_path() / "testCopyTo").string();
}

std::string
copy_path_storage() {
  std::string file_path = __FILE__;
  std::string dir_path = file_path.substr(0, file_path.rfind('\\'));
  if (dir_path.size() == file_path.size())
    dir_path = file_path.substr(0, file_path.rfind('/'));
  return dir_path + "/../../../../scripting_cache/scripts";
}
}// namespace

TEST_CASE("Test register/load player", "[service][arena][script_test]") {

  fys::cache::CmlCopy ccpy(local_path_storage(), copy_path_storage());
  std::filesystem::path baseCache = local_path_storage();

  fys::arena::pit_contenders pc;
  fys::arena::ally_party_teams apt;
  fys::arena::fighting_pit_layout layout(pc, apt);
  auto chai = fys::arena::chai_register::make_chai_instance(pc, apt, layout);
  fys::arena::chai_register::register_base_actions(*chai, ccpy);

  try {
    REQUIRE(chai->eval<bool>(R"(
        var objs = get_objects();
        objs.count("ally_actions") == 1
        )"));
  } catch (std::exception& ex) {
    SPDLOG_ERROR("{}", ex.what());
    FAIL("Shouldn't fail here");
  }

  SECTION("Test copy cml") {

    auto content = ccpy.findInCache("arena:actions:damage:slash.chai");

    REQUIRE(std::filesystem::exists(baseCache));
    REQUIRE(std::filesystem::exists(baseCache / "arena"));
    REQUIRE(std::filesystem::exists(baseCache / "arena" / "actions"));
    REQUIRE(std::filesystem::exists(baseCache / "arena" / "actions" / "damage"));
    REQUIRE(std::filesystem::exists(baseCache / "arena" / "actions" / "damage" / "slash.chai"));
    REQUIRE_FALSE(content.empty());

  }// End section : Test copy cml

  SECTION("simple test load player actions") {

    fys::arena::party_team partyTeam("FyS");
    fys::arena::team_member_sptr tm1 = std::make_shared<fys::arena::team_member>(partyTeam.user_name(), "fyston1");
    tm1->set_id(0);
    tm1->add_doable_action("arena:actions:damage:slash.chai", 1);
    partyTeam.add_team_member(std::move(tm1));

    fys::arena::chai_register::load_register_action_party_team(*chai, ccpy, partyTeam);

    try {
      REQUIRE(chai->eval<bool>(R"(1 == ally_actions.count("FyS_fyston1");)"));
      REQUIRE(chai->eval<bool>(R"(1 == ally_actions["FyS_fyston1"].count("slash");)"));
    } catch (std::exception& ex) {
      SPDLOG_ERROR("{}", ex.what());
      FAIL("Shouldn't fail here");
    }

    SECTION("simple test register same partyteam with loaded action") {

      fys::arena::team_member_sptr tm2 = std::make_shared<fys::arena::team_member>(partyTeam.user_name(), "fyston2");
      tm2->set_id(1);
      tm2->add_doable_action("arena:actions:damage:slash.chai", 1);
      partyTeam.add_team_member(std::move(tm2));

      fys::arena::chai_register::load_register_action_party_team(*chai, ccpy, partyTeam);

      try {
        REQUIRE(chai->eval<bool>(R"(1 == ally_actions.count("FyS_fyston1");)"));
        REQUIRE(chai->eval<bool>(R"(1 == ally_actions["FyS_fyston1"].count("slash");)"));

        REQUIRE(chai->eval<bool>(R"(1 == ally_actions.count("FyS_fyston2");)"));
        REQUIRE(chai->eval<bool>(R"(1 == ally_actions["FyS_fyston2"].count("slash");)"));
      } catch (std::exception& ex) {
        SPDLOG_ERROR("{}", ex.what());
        FAIL("Shouldn't fail here");
      }

    }// End section : Test register loaded action

    SECTION("simple test register new partyteam with loaded action") {

      fys::arena::party_team party_team("Free");
      fys::arena::team_member_sptr tm21 = std::make_shared<fys::arena::team_member>(party_team.user_name(), "fyston1");
      tm21->set_id(2);
      tm21->add_doable_action("arena:actions:damage:slash.chai", 1);
      party_team.add_team_member(std::move(tm21));
      fys::arena::team_member_sptr tm22 = std::make_shared<fys::arena::team_member>(party_team.user_name(), "fyston2");
      tm22->set_id(3);
      tm22->add_doable_action("arena:actions:damage:slash.chai", 1);
      party_team.add_team_member(std::move(tm22));

      fys::arena::chai_register::load_register_action_party_team(*chai, ccpy, party_team);

      try {
        REQUIRE(chai->eval<bool>(R"(1 == ally_actions.count("Free_fyston1");)"));
        REQUIRE(chai->eval<bool>(R"(1 == ally_actions["Free_fyston1"].count("slash");)"));

        REQUIRE(chai->eval<bool>(R"(1 == ally_actions.count("Free_fyston2");)"));
        REQUIRE(chai->eval<bool>(R"(1 == ally_actions["Free_fyston2"].count("slash");)"));
      } catch (std::exception& ex) {
        SPDLOG_ERROR("{}", ex.what());
        FAIL("Shouldn't fail here");
      }

    }// End section : simple test register new partyteam with loaded action

  }// End section : Test load player actions

  std::error_code ec;
  std::filesystem::remove_all(baseCache, ec);

}// End TestCase : Test register/load player
