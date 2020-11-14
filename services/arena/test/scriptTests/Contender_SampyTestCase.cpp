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

#include <arena_server_context.hh>
#include <catch2/catch.hpp>
#include <chai_register.hh>
#include <chaiscript/chaiscript.hpp>
#include <fightingPit/contender/contender_scripting.hh>
#include <fightingPit/contender/fighting_contender.hh>
#include <fightingPit/contender/pit_contenders.hh>
#include <fightingPit/fighting_pit_announcer.hh>
#include <fightingPit/team/ally_party_teams.hh>
#include <fightingPit/team/party_team.hh>
#include <fightingPit/team/team_member.hh>

#include <FSeamMockData.hpp>

#include <CmlCopy.hh>
#include <chai_register.hh>

namespace {
std::string
path_sampy_chai_script() {
  std::string file_path = __FILE__;
  std::string dir_path = file_path.substr(0, file_path.rfind('\\'));
  if (dir_path.size() == file_path.size())
    dir_path = file_path.substr(0, file_path.rfind('/'));
  return dir_path + "/scripts_lnk/arena/contenders/Sampy.chai";
}

std::string
local_path_storage() {
  std::string file_path = __FILE__;
  std::string dir_path = file_path.substr(0, file_path.rfind('\\'));
  if (dir_path.size() == file_path.size())
    dir_path = file_path.substr(0, file_path.rfind('/'));
  return dir_path + "/testCopyTo";
}

std::string
copy_path_storage() {
  std::string file_path = __FILE__;
  std::string dir_path = file_path.substr(0, file_path.rfind('\\'));
  if (dir_path.size() == file_path.size())
    dir_path = file_path.substr(0, file_path.rfind('/'));
  return dir_path + "/scripts_lnk";
}
}// namespace

/**
 * @brief Sampy is a script representing a sample monster and also contains standalone scripts.
 *        It is used in order to test the implementation of ChaiScript int FyS Online.
 */
TEST_CASE("Test Sampy", "[service][arena][script_test]") {

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

  std::string user_name = "FyS";
  fys::arena::party_team_uptr pt = std::make_unique<fys::arena::party_team>(user_name);
  fys::arena::team_member_sptr team_member_1 = std::make_shared<fys::arena::team_member>(user_name, "fyston1");
  fys::arena::team_member_sptr team_member_2 = std::make_shared<fys::arena::team_member>(user_name, "fyston2");

  pt->add_team_member(team_member_1);
  pt->add_team_member(team_member_2);

  fys::arena::fighting_pit_layout::set_contender_initiate_position(*fpc, fys::arena::hexagon_side::orientation::A_N);
  fys::arena::fighting_pit_layout::set_ally_move_initiate_position(*team_member_1, fys::arena::hexagon_side::orientation::A_N);
  fys::arena::fighting_pit_layout::set_ally_move_initiate_position(*team_member_2, fys::arena::hexagon_side::orientation::A_N);

  apt.add_party_team(std::move(pt));

  SECTION("Test initialization") {
    REQUIRE(153 == fpc->access_status().life_pt.current);
    REQUIRE(8 == pc.fighting_contender_at(0)->access_status().initial_speed);
    REQUIRE(153 == pc.fighting_contender_at(0)->access_status().life_pt.current);
    REQUIRE(153 == pc.fighting_contender_at(0)->access_status().life_pt.total);
    REQUIRE(100 == pc.fighting_contender_at(0)->access_status().magic_pt.total);
    REQUIRE(100 == pc.fighting_contender_at(0)->access_status().magic_pt.total);

    // test doable actions
    std::vector<std::string> actionsDoable;
    try {
      actionsDoable = chai->eval<std::vector<std::string>>(R"(
var actions = contender_Sampy0.actions;
retrieveDoableActions(actions.act);)");
    } catch (const std::exception& ee) {
      SPDLOG_ERROR("Error when eval {}", ee.what());
      FAIL("Shouldn't arrive there");
    }

    REQUIRE(3 == actionsDoable.size());
    REQUIRE("test:key:1" == actionsDoable.at(0));
    REQUIRE("test:key:2" == actionsDoable.at(1));
    REQUIRE("test:key:3" == actionsDoable.at(2));
  }

  SECTION("Test Enemy&Attack selection") {
    fpc->access_status().life_pt.current = 16;
    fpc->access_status().life_pt.total = 100;
    team_member_1->access_status().life_pt.current = 10;
    team_member_1->access_status().life_pt.total = 100;
    team_member_2->access_status().life_pt.current = 90;
    team_member_2->access_status().life_pt.total = 110;

    fys::arena::data::priority_elem e{0, 1, true};

    SECTION("Test Action selection") {
      fpc->access_status().life_pt.current = 100;
      fpc->access_status().life_pt.total = 100;
      fpc->access_status().magic_pt.total = 0;

      REQUIRE(100 == fpc->access_status().life_pt.current);
      REQUIRE(100 == fpc->access_status().magic_pt.current);
      pc.execute_contender_action(e);

      REQUIRE(42 == fpc->access_status().magic_pt.current);// Sleeping set the magicPoint to 42

    }// End section : Test baseAttack selection

    SECTION("Test Action Selection BaseAttack") {
      team_member_1->access_status().life_pt.current = 10;
      team_member_1->access_status().life_pt.total = 100;

      REQUIRE(16 == fpc->access_status().life_pt.current);// 16 < ( 50% of 100hp) so baseAttack is decided (50 damage)
      pc.execute_contender_action(e);

      REQUIRE(0 == team_member_1->access_status().life_pt.current);// lower life opponent selected (teamMember1 with 10 life point)
      REQUIRE(team_member_1->access_status().life_pt.is_dead());

    }// Test Action Selection BaseAttack

    SECTION("Test Selection Change of enemy") {
      team_member_2->access_status().life_pt.current = 9;// teamMember2.life.current < teamMember1.life.current

      REQUIRE(16 == fpc->access_status().life_pt.current);// 16 < ( 50% of 100hp) so baseAttack is decided (50 damage)
      pc.execute_contender_action(e);

      REQUIRE(0 == team_member_2->access_status().life_pt.current);// lower life opponent selected (teamMember1 with 10 life point)
      REQUIRE(team_member_2->access_status().life_pt.is_dead());

    }// Test Action Selection BaseAttack

  }// End section : Test Enemy selection

  SECTION("Test network message") {
    auto fseamMock = FSeam::getDefault<fys::arena::ConnectionHandler>();

    //         fseamMock->expectArg<FSeam::ConnectionHandler::sendMessageToDispatcher>(
    //             FSeam::CustomComparator<zmq::multipart_t &>([](auto && test) {
    //                 // verify multipart message contains the selection of the weakest enemy (teamMember1)
    //                 return true;
    //             }), FSeam::VerifyCompare{1});
    // REQUIRE(fseamMock->verify(FSeam::ConnectionHandler::sendMessageToDispatcher::NAME, 1));

    FSeam::MockVerifier::cleanUp();
    std::filesystem::remove_all(baseCache);

  }// End of Section : Test network message

}// End TestCase : Test Sampy