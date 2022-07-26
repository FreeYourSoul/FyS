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


#include <fightingPit/fighting_pit_announcer.hh>
#include <network/worker_service.hh>

#include <catch2/catch_test_macros.hpp>

#include <FSeamMockData.hpp>

#include "test_type.hh"

namespace {
[[nodiscard]] static std::string
local_path_storage() {
  std::string file_path = __FILE__;
  std::string dir_path = file_path.substr(0, file_path.rfind('\\'));
  if (dir_path.size() == file_path.size())
    dir_path = file_path.substr(0, file_path.rfind('/'));
  return dir_path + "/../../../../scripting_cache/scripts";
}
}// namespace

using namespace fys::arena;

TEST_CASE("WorkerServiceTestCase", "[service][arena]") {
  fys::arena::worker_service ws;
  auto cml = CmlBase(local_path_storage());
  encounter_context ctx;

  SECTION("Failures") {

    REQUIRE(fys::arena::fighting_pit::CREATION_ERROR == ws.add_fighting_pit(nullptr));

  }// End section : Failures

  SECTION("Test addFightingPit") {

    unsigned id1 = ws.add_fighting_pit(std::make_unique<fys::arena::fighting_pit>("1", fys::arena::fighting_pit::EASY));
    REQUIRE(1 == id1);
    REQUIRE("1" == fys::arena::fighting_pit_announcer::creator_user_name(ws.get_fighting_pit_instance(id1)));

    unsigned id2 = ws.add_fighting_pit(std::make_unique<fys::arena::fighting_pit>("2", fys::arena::fighting_pit::EASY));
    REQUIRE(2 == id2);
    REQUIRE("2" == fys::arena::fighting_pit_announcer::creator_user_name(ws.get_fighting_pit_instance(id2)));

    unsigned id3 = ws.add_fighting_pit(std::make_unique<fys::arena::fighting_pit>("3", fys::arena::fighting_pit::EASY));
    REQUIRE(3 == id3);
    REQUIRE("3" == fys::arena::fighting_pit_announcer::creator_user_name(ws.get_fighting_pit_instance(id3)));

    unsigned id4 = ws.add_fighting_pit(std::make_unique<fys::arena::fighting_pit>("4", fys::arena::fighting_pit::EASY));
    REQUIRE(4 == id4);
    REQUIRE("4" == fys::arena::fighting_pit_announcer::creator_user_name(ws.get_fighting_pit_instance(id4)));

  }// End section : Test addFightingPit

  SECTION("Test Player join FightingPit") {
    unsigned id = ws.add_fighting_pit(std::make_unique<fys::arena::fighting_pit>("4", fys::arena::fighting_pit::EASY));
    REQUIRE(1 == id);
    REQUIRE("4" == fys::arena::fighting_pit_announcer::creator_user_name(ws.get_fighting_pit_instance(id)));

    auto& pt = ws.get_fighting_pit_instance(1u);
    REQUIRE(nullptr != pt);

    ws.player_join_fighting_pit(1u, getPartyTeam("TestUser"), cml);
    REQUIRE(1 == pt->ally_party().get_party_teams().size());
    REQUIRE(4 == pt->get_party_team_of_player("TestUser").team_members().size());

    ws.player_join_fighting_pit(1u, getPartyTeam("TestUser2"), cml);
    REQUIRE(2 == pt->ally_party().get_party_teams().size());
    REQUIRE(4 == pt->get_party_team_of_player("TestUser2").team_members().size());

    pt->set_player_readiness("TestUser");
    REQUIRE(pt->is_joinable());
    pt->set_player_readiness("TestUser2");
    REQUIRE_FALSE(pt->is_joinable());

    ws.player_join_fighting_pit(41u, getPartyTeam("NOT_GOOD_FP_ID"), cml);// Not existing fighting pit, partyTeam can't join
    REQUIRE(2 == pt->ally_party().get_party_teams().size());
    REQUIRE_THROWS(pt->get_party_team_of_player("NOT_GOOD_FP_ID"));// Not joinable isn't present in the pit and this has not been added

    ws.player_join_fighting_pit(1u, getPartyTeam("NOT_JOINABLE"), cml);
    REQUIRE(2 == pt->ally_party().get_party_teams().size());
    REQUIRE_THROWS(pt->get_party_team_of_player("NOT_JOINABLE"));// Not joinable isn't present in the pit and this has not been added

  }// End section : Test Player join FightingPit

}// End TestCase : WorkerService test