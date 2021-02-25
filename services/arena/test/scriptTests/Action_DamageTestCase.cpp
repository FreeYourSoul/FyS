// MIT License
//
// Copyright (c) 2021 Quentin Balland
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
#include <memory>

#include <chaiscript/chaiscript.hpp>

#include <fightingPit/contender/contender_scripting.hh>
#include <fightingPit/contender/fighting_contender.hh>
#include <fightingPit/contender/pit_contenders.hh>
#include <fightingPit/team/ally_party_teams.hh>
#include <fightingPit/team/party_team.hh>
#include <fightingPit/team/team_member.hh>

#include <CmlCopy.hh>
#include <chai_register.hh>
#include <fightingPit/fighting_pit_announcer.hh>

namespace {
std::string
path_sampy_chai_script() {
  std::string file_path = __FILE__;
  std::string dir_path = file_path.substr(0, file_path.rfind('\\'));
  if (dir_path.size() == file_path.size())
    dir_path = file_path.substr(0, file_path.rfind('/'));
  return dir_path + "/../../../../scripting_cache/scripts/arena/contenders/Sampy.chai";
}

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

TEST_CASE("test damage chaiscript", "[service][arena][script_test]") {

  fys::cache::CmlCopy ccpy(local_path_storage(), copy_path_storage());
  std::filesystem::path baseCache = local_path_storage();

  fys::arena::pit_contenders pc;
  fys::arena::ally_party_teams apt;
  fys::arena::fighting_pit_layout layout(pc, apt);
  auto chai = fys::arena::chai_register::make_chai_instance(pc, apt, layout);

  fys::arena::chai_register::register_base_actions(*chai, ccpy);

  fys::arena::ContenderScriptingUPtr sampy = std::make_unique<fys::arena::contender_scripting>(*chai, 1);
  sampy->set_contender_id(0u);
  sampy->set_contender_name("Sampy");
  sampy->load_contender_script_from_file(path_sampy_chai_script());

  auto fpc = std::make_shared<fys::arena::fighting_contender>(std::move(sampy));
  REQUIRE(pc.add_contender(fpc));

  SECTION("Test initialization contender") {
    REQUIRE(153 == fpc->access_status().life_pt.current);
    REQUIRE(8 == pc.fighting_contender_at(0)->access_status().initial_speed);
    REQUIRE(153 == pc.fighting_contender_at(0)->access_status().life_pt.current);
    REQUIRE(153 == pc.fighting_contender_at(0)->access_status().life_pt.total);
    REQUIRE(100 == pc.fighting_contender_at(0)->access_status().magic_pt.total);
    REQUIRE(100 == pc.fighting_contender_at(0)->access_status().magic_pt.total);
  }

  fys::arena::party_team party_team("FyS");
  fys::arena::team_member_sptr tm1 = std::make_shared<fys::arena::team_member>(party_team.user_name(), "fyston1");
  auto& st = tm1->access_status();
  st.initial_speed = 100;
  st.magic_pt = {1337, 1337};
  st.life_pt = {42, 42};

  SECTION("test Slash chaiscript") {

    tm1->add_doable_action("arena:actions:damage:slash.chai", 1);
    party_team.add_team_member(std::move(tm1));

    fys::arena::chai_register::load_register_action_party_team(*chai, ccpy, party_team);

    SECTION("test damage") {

      try {
        chai->eval(R"(
var &contender = pitContenders.getFightingContender(0);
var s = slash(1);
s.execute(contender);
)");
      } catch (const chaiscript::exception::eval_error& ex) {
        SPDLOG_ERROR("{}", ex.pretty_print());
        FAIL("Chaiscript : Shouldn't fail here");
      }
      REQUIRE(120 == pc.fighting_contender_at(0)->access_status().life_pt.current);// -33 life
      REQUIRE(153 == pc.fighting_contender_at(0)->access_status().life_pt.total);
      REQUIRE(1 == pc.fighting_contender_at(0)->status().alteration_after.size());
      auto& stat = pc.fighting_contender_at(0)->access_status();
      auto alt = stat.alteration_after.at(0);
      REQUIRE(2 == alt.turn());
      alt.process_alteration(stat);
      REQUIRE(1 == alt.turn());
    }// End section : test damage

    SECTION("test over damage") {

      pc.fighting_contender_at(0)->access_status().life_pt.current = 10;

      try {
        chai->eval(R"(
var &contender = pitContenders.getFightingContender(0);
var s = slash(1);
s.execute(contender);
)");
      } catch (std::exception& ex) {
        SPDLOG_ERROR("{}", ex.what());
        FAIL("Chaiscript : Shouldn't fail here");
      }
      REQUIRE(0 == pc.fighting_contender_at(0)->access_status().life_pt.current);// -33 life > 0
      REQUIRE(153 == pc.fighting_contender_at(0)->access_status().life_pt.total);

    }// End section : test over damage

  }// End section : test Slash chaiscript

  SECTION("test Multi_Slash chaiscript") {

    tm1->add_doable_action("arena:actions:damage:multi_slash.chai", 1);
    party_team.add_team_member(std::move(tm1));

    fys::arena::ContenderScriptingUPtr sampy2 = std::make_unique<fys::arena::contender_scripting>(*chai, 1);
    sampy2->set_contender_id(1u);
    sampy2->set_contender_name("Sampy");
    sampy2->load_contender_script_from_file(path_sampy_chai_script());

    auto fpc2 = std::make_shared<fys::arena::fighting_contender>(std::move(sampy2));
    REQUIRE(pc.add_contender(fpc2));

    fys::arena::chai_register::load_register_action_party_team(*chai, ccpy, party_team);

    pc.fighting_contender_at(1)->access_status().life_pt.current = 100;

    SECTION("test damage") {

      try {
        chai->eval(R"(
var &contenderStatus = pitContenders.getFightingContender(0);
var &contender2Status = pitContenders.getFightingContender(1);
var s = multi_slash(1);
var d = [];
d.push_back_ref(contenderStatus);
d.push_back_ref(contender2Status);
s.execute(d);
)");
      } catch (std::exception& ex) {
        SPDLOG_ERROR("{}", ex.what());
        FAIL("Chaiscript : Shouldn't fail here");
      }
      REQUIRE(123 == pc.fighting_contender_at(0)->access_status().life_pt.current);// -33 life
      REQUIRE(153 == pc.fighting_contender_at(0)->access_status().life_pt.total);

      REQUIRE(70 == pc.fighting_contender_at(1)->access_status().life_pt.current);// -33 life
      REQUIRE(153 == pc.fighting_contender_at(1)->access_status().life_pt.total);

    }// End section : test damage

    SECTION("test over damage") {

      pc.fighting_contender_at(0)->access_status().life_pt.current = 10;

      try {
        chai->eval(R"(
var &contenderStatus = pitContenders.getFightingContender(0);
var &contender2Status = pitContenders.getFightingContender(1);
var s = multi_slash(1);
var d = [];
d.push_back_ref(contenderStatus);
d.push_back_ref(contender2Status);
s.execute(d);
)");
      } catch (std::exception& ex) {
        SPDLOG_ERROR("{}", ex.what());
        FAIL("Chaiscript : Shouldn't fail here");
      }

      REQUIRE(0 == pc.fighting_contender_at(0)->access_status().life_pt.current);// -33 life > 0
      REQUIRE(153 == pc.fighting_contender_at(0)->access_status().life_pt.total);

      REQUIRE(70 == pc.fighting_contender_at(1)->access_status().life_pt.current);// -33 life
      REQUIRE(153 == pc.fighting_contender_at(1)->access_status().life_pt.total);

    }// End section : test over damage

    std::error_code ec;
    std::filesystem::remove_all(baseCache, ec);

  }// End section : test Multi_Slash chaiscript

}// End TestCase : test damage chaiscript