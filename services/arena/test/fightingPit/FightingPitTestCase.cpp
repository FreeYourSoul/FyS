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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-msc32-c"

#include <arena_server_context.hh>
#include <catch2/catch.hpp>
#include <fightingPit/fighting_pit_announcer.hh>

#include "TestType.hh"
#include <connection_handler.hh>
#include <util/ChaiUtility.hh>

#include <fightingPit/contender/contender_scripting.hh>
#include <fightingPit/contender/fighting_contender.hh>

using namespace fys::arena;
using namespace std::chrono_literals;

namespace {

[[nodiscard]] static std::string
local_path_storage() {
  std::string file_path = __FILE__;
  std::string dir_path = file_path.substr(0, file_path.rfind('\\'));
  if (dir_path.size() == file_path.size())
	dir_path = file_path.substr(0, file_path.rfind('/'));
  return dir_path + "/../scriptTests/scripts_lnk";
}

[[nodiscard]] std::string
getTmpPath() {
  std::string file_path = __FILE__;
  std::string dir_path = file_path.substr(0, file_path.rfind('\\'));
  if (dir_path.size() == file_path.size())
	dir_path = file_path.substr(0, file_path.rfind('/'));
  return dir_path + "/testCopyTo";
}

}// namespace

// Test of the game loop

TEST_CASE("FightingPitTestCase Simple Fight test", "[service][arena]") {
  auto fseamMock = FSeam::getDefault<fys::util::random_generator>();
  fys::common::connection_handler handler{};
  auto fseamConnectionHandlerMock = FSeam::get(&handler);
  auto cml = CmlBaseCopy(getTmpPath(), local_path_storage());
  DeleteFolderWhenDone raii_delete_folder(getTmpPath());

  encounter_context ctx;
  ctx.range_encounter_per_zone["WS00"] = {
	  std::pair(1, 1),// ez
	  std::pair(3, 3),// medium
	  std::pair(1, 1) // hard
  };
  ctx.contenders_per_zone["WS00"] = {
	  encounter_context::encounter_desc{
		  "testing:TestMonsterSleep.chai",
		  3,
		  {100, 100, 100},
		  std::pair(1u, 10u)},
  };

  // seed used 42
  std::shared_ptr<std::mt19937> mt = std::make_shared<std::mt19937>(42);
  fseamMock->dupeReturn<FSeam::random_generator::get>(mt);

  SECTION("InGame fighting Ally Team Win (One Contender)") {

	fighting_pit_announcer fpa(cml);
	fpa.set_creator_user_name("Winner");
	fpa.set_creator_user_token("WinnerToken");
	fpa.set_creator_team_party(getPartyTeam("Winner"));
	fpa.set_difficulty(fighting_pit::level::EASY);
	fpa.set_encounter_type(fighting_pit_announcer::encounter_type::RANDOM);

	fpa.add_action_to_one_member(0u, "arena:actions:damage:slash.chai", 1u);// slash level 1 do 33 damage
	fpa.add_action_to_one_member(1u, "arena:actions:damage:slash.chai", 2u);// slash level 2 do 36 damage
	fpa.add_action_to_one_member(2u, "arena:actions:damage:slash.chai", 3u);// slash level 3 do 39 damage
	fpa.add_action_to_one_member(3u, "arena:actions:damage:slash.chai", 4u);// slash level 4 do 42 damage

	auto fp = fpa.build_fighting_pit(ctx, "WS00");

	auto& chai = fp->get_chai_ptr();

	const auto& contender = fighting_pit_announcer::get_pit_contenders(fp).contenders().at(0);
	REQUIRE(1 == fighting_pit_announcer::get_pit_contenders(fp).number_contender());
	REQUIRE("TestMonsterSleep" == contender->name());
	REQUIRE(100 == contender->status().life_pt.current);
	REQUIRE(100 == contender->status().life_pt.total);

	const auto& c = fighting_pit_announcer::get_pit_contenders(fp).contenders_on_side(hexagon_side::orientation::B_S);
	const auto& p = fighting_pit_announcer::get_party_teams(fp).members_by_side(hexagon_side::orientation::B_S);

	REQUIRE(fp->is_joinable());
	fp->set_player_readiness("Winner");
	REQUIRE_FALSE(fp->is_joinable());
	REQUIRE(fighting_pit_announcer::is_on_going(fp));

	SECTION("Battle Turn 1 test") {

	  p.at(0)->add_pending_action("slash", contender_target_id{0});
	  p.at(1)->add_pending_action("slash", contender_target_id{0});
	  p.at(2)->add_pending_action("slash", contender_target_id{0});
	  p.at(3)->add_pending_action("slash", contender_target_id{0});

	  auto now = std::chrono::system_clock::now();

	  try {
		// Player id 4 (at index 3) is going to slash the only opponent it has inflicting 42 damage
		// The ennemy total life is 100, goes to 58
		fp->continue_battle(now);
		REQUIRE(58 == fighting_pit_announcer::get_pit_contenders(fp).contenders().at(0)->status().life_pt.current);
		REQUIRE(100 == fighting_pit_announcer::get_pit_contenders(fp).contenders().at(0)->status().life_pt.total);

		// Player id 3 (at index 2) is going to slash the only opponent it has inflicting 39 damage
		// The ennemy total life is 100, current from 58 goes to 19
		now += fys::arena::interval::EASY + 1ms;
		fp->continue_battle(now);
		REQUIRE(19 == fighting_pit_announcer::get_pit_contenders(fp).contenders().at(0)->status().life_pt.current);
		REQUIRE(100 == fighting_pit_announcer::get_pit_contenders(fp).contenders().at(0)->status().life_pt.total);

		// Contender id 0 is going to Sleep and so restore 40 hp
		// The ennemy total life is 100, current from 19 goes to 59
		now += fys::arena::interval::EASY + 1ms;
		fp->continue_battle(now);
		REQUIRE(59 == fighting_pit_announcer::get_pit_contenders(fp).contenders().at(0)->status().life_pt.current);
		REQUIRE(100 == fighting_pit_announcer::get_pit_contenders(fp).contenders().at(0)->status().life_pt.total);

		// Player id 2 (at index 1) is going to slash the only opponent it has inflicting 36 damage
		// The ennemy total life is 100, current from 59 goes to 23
		now += fys::arena::interval::EASY + 1ms;
		fp->continue_battle(now);
		REQUIRE(23 == fighting_pit_announcer::get_pit_contenders(fp).contenders().at(0)->status().life_pt.current);
		REQUIRE(100 == fighting_pit_announcer::get_pit_contenders(fp).contenders().at(0)->status().life_pt.total);

		// Player id 2 (at index 1) is going to slash the only opponent it has inflicting 33 damage
		// The ennemy total life is 100, current from 23 goes to 0 (overkill)
		now += fys::arena::interval::EASY + 1ms;
		REQUIRE(fighting_pit_announcer::is_on_going(fp));
		fp->continue_battle(now);
		REQUIRE(0 == fighting_pit_announcer::get_pit_contenders(fp).contenders().at(0)->status().life_pt.current);
		REQUIRE(100 == fighting_pit_announcer::get_pit_contenders(fp).contenders().at(0)->status().life_pt.total);

		// Battle ended checks
		REQUIRE_FALSE(fighting_pit_announcer::is_on_going(fp));
		REQUIRE(fighting_pit_announcer::is_ally_win(fp));

		// Battle is ended and so nothing happens
		// todo check the call to connection manager with the reply latter on
		now += fys::arena::interval::EASY + 1ms;
		fp->continue_battle(now);

	  } catch (const std::exception& e) {
		SPDLOG_ERROR("Exception during Battle turn 1 test {} ", e.what());
		FAIL("Should not arrive here");
	  }

	}// End section : Battle Turn 1 test

  }// End section : InGame fighting

  SECTION("InGame fighting Contender Team Win (One Contender)") {

	ctx.contenders_per_zone["WS00"] = {
		encounter_context::encounter_desc{
			"testing:TestMonsterAttack.chai",
			3,
			{100, 100, 100},
			std::pair(1u, 10u)},
	};

	fighting_pit_announcer fpa(cml);
	fpa.set_creator_user_name("Loser");
	fpa.set_creator_user_token("LoserToken");
	auto partyTeam = getPartyTeam("Loser");
	partyTeam->access_team_members().at(0)->access_status().magic_pt.current = 0;
	partyTeam->access_team_members().at(1)->access_status().magic_pt.current = 0;
	partyTeam->access_team_members().at(2)->access_status().magic_pt.current = 0;
	partyTeam->access_team_members().at(2)->access_status().life_pt.current = 99;// set to the lowest life in order to ensure as target
	partyTeam->access_team_members().at(3)->access_status().magic_pt.current = 0;
	fpa.set_creator_team_party(std::move(partyTeam));
	fpa.set_difficulty(fighting_pit::level::EASY);
	fpa.set_encounter_type(fighting_pit_announcer::encounter_type::RANDOM);

	fpa.add_action_to_one_member(0u, "arena:actions:misc:meditate.chai", 1u);// meditate level 1 restore 10 mp
	fpa.add_action_to_one_member(1u, "arena:actions:misc:meditate.chai", 2u);// meditate level 2 restore 20 mp
	fpa.add_action_to_one_member(2u, "arena:actions:misc:meditate.chai", 3u);// meditate level 3 restore 30 mp
	fpa.add_action_to_one_member(3u, "arena:actions:misc:meditate.chai", 4u);// meditate level 4 restore 40 mp

	auto fp = fpa.build_fighting_pit(ctx, "WS00");

	auto& chai = fp->get_chai_ptr();

	REQUIRE(1 == fighting_pit_announcer::get_pit_contenders(fp).number_contender());
	const auto& contender = fighting_pit_announcer::get_pit_contenders(fp).contenders().at(0);
	const auto& c = fighting_pit_announcer::get_pit_contenders(fp).contenders_on_side(hexagon_side::orientation::B_S);
	const auto& p = fighting_pit_announcer::get_party_teams(fp).members_by_side(hexagon_side::orientation::B_S);

	SECTION("Initial setup") {
	  REQUIRE("TestMonsterAttack" == contender->name());
	  REQUIRE(4 == fp->get_party_team_of_player("Loser").team_members().size());
	  REQUIRE(100 == fp->get_party_team_of_player("Loser").team_members()[0]->status().life_pt.current);
	  REQUIRE(100 == fp->get_party_team_of_player("Loser").team_members()[0]->status().life_pt.total);
	  REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);
	  REQUIRE(200 == fp->get_party_team_of_player("Loser").team_members()[1]->status().life_pt.current);
	  REQUIRE(200 == fp->get_party_team_of_player("Loser").team_members()[1]->status().life_pt.total);
	  REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);
	  REQUIRE(99 == fp->get_party_team_of_player("Loser").team_members()[2]->status().life_pt.current);// <<< LOWEST LIFE TARGET
	  REQUIRE(550 == fp->get_party_team_of_player("Loser").team_members()[2]->status().life_pt.total);
	  REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);
	  REQUIRE(140 == fp->get_party_team_of_player("Loser").team_members()[3]->status().life_pt.current);
	  REQUIRE(140 == fp->get_party_team_of_player("Loser").team_members()[3]->status().life_pt.total);
	  REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);

	  REQUIRE(1 == fighting_pit_announcer::get_pit_contenders(fp).number_contender());
	  REQUIRE(1 == fighting_pit_announcer::get_pit_contenders(fp).number_contender_on_side(hexagon_side::orientation::B_S));
	  REQUIRE(4 == fighting_pit_announcer::get_party_teams(fp).number_ally());

	  REQUIRE(8 == c.at(0)->status().initial_speed);
	  REQUIRE(3 == p.at(0)->status().initial_speed);
	  REQUIRE(5 == p.at(1)->status().initial_speed);
	  REQUIRE(10 == p.at(2)->status().initial_speed);
	  REQUIRE(20 == p.at(3)->status().initial_speed);
	}// End section : Initial setup

	SECTION("Test ChaiUtility memberHasActionRegistered") {
	  try {
		auto& memberId1 = fp->get_party_team_of_player("Loser").team_members()[0];
		REQUIRE(chai::util::member_has_action_registered(*fp->get_chai_ptr(),
														 memberId1->user_name(), memberId1->name(), "meditate"));
		REQUIRE_FALSE(chai::util::member_has_action_registered(*fp->get_chai_ptr(),
															   memberId1->user_name(), memberId1->name(), "DONOTEXIST"));
		REQUIRE_FALSE(chai::util::member_has_action_registered(*fp->get_chai_ptr(),
															   "unknown", "meh", "DONOTEXIST"));
	  } catch (const std::exception& ee) {
		FAIL(ee.what());
	  }
	}// End section : Test ChaiUtility memberHasActionRegistered

	REQUIRE(fp->is_joinable());
	fp->set_player_readiness("Loser");
	REQUIRE_FALSE(fp->is_joinable());
	REQUIRE(fighting_pit_announcer::is_on_going(fp));

	// other turns are tested in the test  Test for FightingPit testcase in file PriorityOrderListTestCase.cpp

	SECTION("Battle Turn 1 test") {

	  p.at(0)->add_pending_action("meditate", std::nullopt);
	  p.at(1)->add_pending_action("meditate", std::nullopt);
	  p.at(2)->add_pending_action("meditate", std::nullopt);
	  p.at(3)->add_pending_action("meditate", std::nullopt);

	  auto now = std::chrono::system_clock::now();

	  try {
		// Player id 4 (at index 3) is going to meditate, restore 40 mp, from 0 mp to 10 (max mp 10)
		fp->continue_battle(now);
		REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[3]->status().magic_pt.current);
		REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[2]->status().magic_pt.current);
		REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[1]->status().magic_pt.current);
		REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);

		// Player id 3 (at index 2) is going to meditate, restore 30 mp, from 0 mp to 10 (max mp 10)
		now += fys::arena::interval::EASY + 1ms;
		fp->continue_battle(now);
		REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[2]->status().magic_pt.current);
		REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[1]->status().magic_pt.current);
		REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);

		// Contender turn : Target lowest life opponent:
		// ally id 1  =  100
		// ally id 2  =  200
		// ally id 3  =  99  << Target -> damage done by attack 90 damage << 9 life left
		// ally id 4  =  140
		now += fys::arena::interval::EASY + 1ms;
		fp->continue_battle(now);
		REQUIRE(9 == fp->get_party_team_of_player("Loser").team_members()[2]->status().life_pt.current);// damage inflicted
		REQUIRE(550 == fp->get_party_team_of_player("Loser").team_members()[2]->status().life_pt.total);

		// Stay same
		REQUIRE(100 == fp->get_party_team_of_player("Loser").team_members()[0]->status().life_pt.current);
		REQUIRE(100 == fp->get_party_team_of_player("Loser").team_members()[0]->status().life_pt.total);
		REQUIRE(200 == fp->get_party_team_of_player("Loser").team_members()[1]->status().life_pt.current);
		REQUIRE(200 == fp->get_party_team_of_player("Loser").team_members()[1]->status().life_pt.total);
		REQUIRE(140 == fp->get_party_team_of_player("Loser").team_members()[3]->status().life_pt.current);
		REQUIRE(140 == fp->get_party_team_of_player("Loser").team_members()[3]->status().life_pt.total);

		// Player id 2 (at index 1) is going to meditate, restore 20 mp, from 0 mp to 0 (max mp 0)
		now += fys::arena::interval::EASY + 1ms;
		fp->continue_battle(now);
		REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[1]->status().magic_pt.current);
		REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);

		// Player id 1 (at index 0) is going to meditate, restore 10 mp, from 0 mp to 10 (max mp 20)
		now += fys::arena::interval::EASY + 1ms;
		REQUIRE(fighting_pit_announcer::is_on_going(fp));
		fp->continue_battle(now);
		REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);

		SECTION("Battle Turn 2 test") {// a4, a4, a3, c0, a2, c0, a1
		  p.at(1)->add_pending_action("meditate", std::nullopt);
		  p.at(2)->add_pending_action("meditate", std::nullopt);
		  p.at(3)->add_pending_action("meditate", std::nullopt);

		  // Player id 4 (at index 3) is going to meditate, restore 20 mp, from 10 mp to 10 (max mp 10)
		  now += fys::arena::interval::EASY + 1ms;
		  fp->continue_battle(now);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[3]->status().magic_pt.current);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[2]->status().magic_pt.current);
		  REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[1]->status().magic_pt.current);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);

		  // Player id 4 (at index 3) is going to meditate, restore 20 mp, from 10 mp to 10 (max mp 10)
		  now += fys::arena::interval::EASY + 1ms;
		  fp->continue_battle(now);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[3]->status().magic_pt.current);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[2]->status().magic_pt.current);
		  REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[1]->status().magic_pt.current);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);

		  // Player id 3 (at index 2) is going to meditate, restore 30 mp, from 10 mp to 10 (max mp 10)
		  now += fys::arena::interval::EASY + 1ms;
		  fp->continue_battle(now);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[3]->status().magic_pt.current);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[2]->status().magic_pt.current);
		  REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[1]->status().magic_pt.current);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);

		  // Contender turn : Target lowest life opponent:
		  // ally id 1  =  100
		  // ally id 2  =  200
		  // ally id 3  =  9   << Target -> damage done by attack 90 damage << 0 life left (DEAD)
		  // ally id 4  =  140
		  now += fys::arena::interval::EASY + 1ms;
		  fp->continue_battle(now);
		  REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[2]->status().life_pt.current);
		  REQUIRE(fp->get_party_team_of_player("Loser").team_members()[2]->status().life_pt.is_dead());
		  // Stay same
		  REQUIRE(100 == fp->get_party_team_of_player("Loser").team_members()[0]->status().life_pt.current);// damage inflicted
		  REQUIRE(100 == fp->get_party_team_of_player("Loser").team_members()[0]->status().life_pt.total);
		  REQUIRE(200 == fp->get_party_team_of_player("Loser").team_members()[1]->status().life_pt.current);
		  REQUIRE(200 == fp->get_party_team_of_player("Loser").team_members()[1]->status().life_pt.total);
		  REQUIRE(550 == fp->get_party_team_of_player("Loser").team_members()[2]->status().life_pt.total);
		  REQUIRE(140 == fp->get_party_team_of_player("Loser").team_members()[3]->status().life_pt.current);
		  REQUIRE(140 == fp->get_party_team_of_player("Loser").team_members()[3]->status().life_pt.total);
		  // Player died and is not in game anymore, he won't play next turn

		  // Player id 2 (at index 1) is going to meditate, restore 10 mp, from 0 mp to 0 (max mp 0)
		  now += fys::arena::interval::EASY + 1ms;
		  fp->continue_battle(now);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[3]->status().magic_pt.current);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[2]->status().magic_pt.current);
		  REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[1]->status().magic_pt.current);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);

		  // Contender turn : Target lowest life opponent:
		  // ally id 1  =  100 << target from 100 to 10
		  // ally id 2  =  200
		  // ally id 3  =  0 (DEAD)
		  // ally id 4  =  140
		  now += fys::arena::interval::EASY + 1ms;
		  fp->continue_battle(now);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[0]->status().life_pt.current);// damage inflicted
		  REQUIRE(100 == fp->get_party_team_of_player("Loser").team_members()[0]->status().life_pt.total);
		  // Stay same
		  REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[2]->status().life_pt.current);
		  REQUIRE(fp->get_party_team_of_player("Loser").team_members()[2]->status().life_pt.is_dead());
		  REQUIRE(200 == fp->get_party_team_of_player("Loser").team_members()[1]->status().life_pt.current);
		  REQUIRE(200 == fp->get_party_team_of_player("Loser").team_members()[1]->status().life_pt.total);
		  REQUIRE(550 == fp->get_party_team_of_player("Loser").team_members()[2]->status().life_pt.total);
		  REQUIRE(140 == fp->get_party_team_of_player("Loser").team_members()[3]->status().life_pt.current);
		  REQUIRE(140 == fp->get_party_team_of_player("Loser").team_members()[3]->status().life_pt.total);

		  // Player id 1 (at index 0) is going to meditate, restore 10 mp, from 10 mp to 10 (max mp 10)
		  now += fys::arena::interval::EASY + 1ms;
		  fp->continue_battle(now);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[3]->status().magic_pt.current);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[2]->status().magic_pt.current);
		  REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[1]->status().magic_pt.current);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);

		  SECTION("Battle turn 3 test") {
			p.at(0)->add_pending_action("meditate", std::nullopt);
			p.at(2)->add_pending_action("meditate", std::nullopt);
			p.at(3)->add_pending_action("meditate", std::nullopt);

			// Player id 4 (at index 3)
			now += fys::arena::interval::EASY + 1ms;
			fp->continue_battle(now);

			// Contender
			now += fys::arena::interval::EASY + 1ms;
			fp->continue_battle(now);
			REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[0]->status().life_pt.current);// damage inflicted
			REQUIRE(fp->get_party_team_of_player("Loser").team_members()[0]->status().life_pt.is_dead());
			REQUIRE(100 == fp->get_party_team_of_player("Loser").team_members()[0]->status().life_pt.total);
			// Stay same
			REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[2]->status().life_pt.current);
			REQUIRE(fp->get_party_team_of_player("Loser").team_members()[2]->status().life_pt.is_dead());
			REQUIRE(200 == fp->get_party_team_of_player("Loser").team_members()[1]->status().life_pt.current);
			REQUIRE(200 == fp->get_party_team_of_player("Loser").team_members()[1]->status().life_pt.total);
			REQUIRE(550 == fp->get_party_team_of_player("Loser").team_members()[2]->status().life_pt.total);
			REQUIRE(140 == fp->get_party_team_of_player("Loser").team_members()[3]->status().life_pt.current);
			REQUIRE(140 == fp->get_party_team_of_player("Loser").team_members()[3]->status().life_pt.total);

			// Player id 2 (at index 1)
			now += fys::arena::interval::EASY + 1ms;
			fp->continue_battle(now);

			// Player id 4 (at index 1) is going to meditate, restore 10 mp, from 0 mp to 0 (max mp 0)
			now += fys::arena::interval::EASY + 1ms;
			fp->continue_battle(now);

			// Player id 1 (at index 0) is going to meditate, restore 10 mp, from 20 mp to 20 (max mp 20)
			now += fys::arena::interval::EASY + 1ms;
			fp->continue_battle(now);

		  }// End section : Battle turn 3 test

		}// End section : Battle Turn 2 test

		//				// Battle ended checks
		//				REQUIRE_FALSE(FightingPitAnnouncer::isOnGoing(fp));
		//				REQUIRE(FightingPitAnnouncer::isContenderWin(fp));
		//
		//				// Battle is ended and so nothing happens
		//				// todo check the call to connection manager with the reply latter on
		//				now += fys::arena::interval::EASY + 1ms;
		//				fp->continueBattle(now);
		//
	  } catch (const std::exception& e) {
		SPDLOG_ERROR("Exception during Battle turn 1 test {} ", e.what());
		FAIL("Should not arrive here");
	  }

	}// End section : Battle Turn 1 test

  }// End section : InGame fighting

  // cleanup
  FSeam::MockVerifier::cleanUp();

}// End TestCase : FightingPit Simple Fight test

TEST_CASE("FightingPitTestCase Move test", "[service][arena]") {
  auto fseamMock = FSeam::getDefault<fys::util::random_generator>();
  fys::common::connection_handler handler{};
  auto fseamConnectionHandlerMock = FSeam::get(&handler);
  auto cml = CmlBaseCopy(getTmpPath(), local_path_storage());
  DeleteFolderWhenDone raii_delete_folder(getTmpPath());

  encounter_context ctx;
  ctx.range_encounter_per_zone["WS00"] = {
	  std::pair(1, 1),// ez
	  std::pair(3, 3),// medium
	  std::pair(1, 1) // hard
  };
  ctx.contenders_per_zone["WS00"] = {
	  encounter_context::encounter_desc{
		  "testing:TestMonsterMove.chai",
		  2,
		  {100, 100, 100},
		  std::pair(1u, 10u)},
  };

  // seed used 42
  std::shared_ptr<std::mt19937> mt = std::make_shared<std::mt19937>(42);
  fseamMock->dupeReturn<FSeam::random_generator::get>(mt);

  SECTION("InGame MoveTestCase") {

	fighting_pit_announcer fpa(cml);
	fpa.set_creator_user_name("Loser");
	fpa.set_creator_user_token("LoserToken");
	auto partyTeam = getPartyTeam("Loser");
	partyTeam->access_team_members().at(0)->access_status().magic_pt.current = 0;
	partyTeam->access_team_members().at(1)->access_status().magic_pt.current = 0;
	partyTeam->access_team_members().at(2)->access_status().magic_pt.current = 0;
	partyTeam->access_team_members().at(2)->access_status().life_pt.current = 99;// set to the lowest life in order to ensure as target
	partyTeam->access_team_members().at(3)->access_status().magic_pt.current = 0;
	fpa.set_creator_team_party(std::move(partyTeam));
	fpa.set_difficulty(fighting_pit::level::EASY);
	fpa.set_encounter_type(fighting_pit_announcer::encounter_type::RANDOM);

	auto fp = fpa.build_fighting_pit(ctx, "WS00");

	auto& chai = fp->get_chai_ptr();

	REQUIRE(1 == fighting_pit_announcer::get_pit_contenders(fp).number_contender());
	const auto& contender = fighting_pit_announcer::get_pit_contenders(fp).contenders().at(0);
	const auto& c = fighting_pit_announcer::get_pit_contenders(fp).contenders_on_side(hexagon_side::orientation::B_S);
	const auto& p = fighting_pit_announcer::get_party_teams(fp).members_by_side(hexagon_side::orientation::B_S);

	SECTION("Initial setup") {
	  REQUIRE("TestMonsterMove" == contender->name());
	  REQUIRE(4 == fp->get_party_team_of_player("Loser").team_members().size());
	  REQUIRE(100 == fp->get_party_team_of_player("Loser").team_members()[0]->status().life_pt.current);
	  REQUIRE(100 == fp->get_party_team_of_player("Loser").team_members()[0]->status().life_pt.total);
	  REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);
	  REQUIRE(200 == fp->get_party_team_of_player("Loser").team_members()[1]->status().life_pt.current);
	  REQUIRE(200 == fp->get_party_team_of_player("Loser").team_members()[1]->status().life_pt.total);
	  REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);
	  REQUIRE(99 == fp->get_party_team_of_player("Loser").team_members()[2]->status().life_pt.current);// <<< LOWEST LIFE TARGET
	  REQUIRE(550 == fp->get_party_team_of_player("Loser").team_members()[2]->status().life_pt.total);
	  REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);
	  REQUIRE(140 == fp->get_party_team_of_player("Loser").team_members()[3]->status().life_pt.current);
	  REQUIRE(140 == fp->get_party_team_of_player("Loser").team_members()[3]->status().life_pt.total);
	  REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);

	  REQUIRE(1 == fighting_pit_announcer::get_pit_contenders(fp).number_contender());
	  REQUIRE(1 == fighting_pit_announcer::get_pit_contenders(fp).number_contender_on_side(hexagon_side::orientation::B_S));
	  REQUIRE(4 == fighting_pit_announcer::get_party_teams(fp).number_ally());

	  const auto& doableActions =
		  fighting_pit_announcer::get_pit_contenders(fp).contenders().at(0)->get_contender_scripting()->doable_actions();
	  REQUIRE(1 == doableActions.size());
	  REQUIRE("test:key:1" == doableActions.at(0));

	  REQUIRE(8 == c.at(0)->status().initial_speed);
	  REQUIRE(3 == p.at(0)->status().initial_speed);
	  REQUIRE(5 == p.at(1)->status().initial_speed);
	  REQUIRE(10 == p.at(2)->status().initial_speed);
	  REQUIRE(20 == p.at(3)->status().initial_speed);
	}// End section : Initial setup

	REQUIRE(fp->is_joinable());
	fp->set_player_readiness("Loser");
	REQUIRE_FALSE(fp->is_joinable());
	REQUIRE(fighting_pit_announcer::is_on_going(fp));

	// other turns are tested in the test  Test for FightingPit testcase in file PriorityOrderListTestCase.cpp

	SECTION("Battle Turn 1 test") {

	  auto now = std::chrono::system_clock::now();

	  try {
		// Player id 4 (at index 3)
		fp->continue_battle(now);

		// Player id 3 (at index 2)
		now += fys::arena::interval::EASY + 1ms;
		fp->continue_battle(now);

		// Contender turn : Move from B_S to A_NE
		now += fys::arena::interval::EASY + 1ms;
		REQUIRE(hexagon_side::orientation::B_S == fighting_pit_announcer::get_pit_contenders(fp).contenders().at(0)->side_orient());
		fp->continue_battle(now);
		REQUIRE(hexagon_side::orientation::A_NE == fighting_pit_announcer::get_pit_contenders(fp).contenders().at(0)->side_orient());

		// second move occurs as the contender is now on a new side where no other player are.
		// And so alone, so he takes his turn

		now += fys::arena::interval::EASY + 1ms;
		fp->continue_battle(now);
		REQUIRE(hexagon_side::orientation::C_NE == fighting_pit_announcer::get_pit_contenders(fp).contenders().at(0)->side_orient());

		//				now += fys::arena::interval::EASY + 1ms;
		//				fp->continueBattle(now);
		//				REQUIRE(HexagonSide::Orientation::A_NE == FightingPitAnnouncer::getPitContenders(fp).getContenders().at(0)->getHexagonSideOrient());

	  } catch (const std::exception& e) {
		SPDLOG_ERROR("Exception during Battle turn 1 test {} ", e.what());
		FAIL("Should not arrive here");
	  }

	}// End section : Battle Turn 1 test

  }// End section : InGame fighting

  // cleanup
  FSeam::MockVerifier::cleanUp();

}// End TestCase : FightingPit Move test

TEST_CASE("FightingPitTestCase Simple Alteration test", "[service][arena]") {
  auto fseamMock = FSeam::getDefault<fys::util::random_generator>();
  fys::common::connection_handler handler{};
  auto fseamConnectionHandlerMock = FSeam::get(&handler);
  auto cml = CmlBaseCopy(getTmpPath(), local_path_storage());
  DeleteFolderWhenDone raii_delete_folder(getTmpPath());

  encounter_context ctx;
  ctx.range_encounter_per_zone["WS00"] = {
	  std::pair(1, 1),// ez
	  std::pair(3, 3),// medium
	  std::pair(1, 1) // hard
  };
  ctx.contenders_per_zone["WS00"] = {
	  encounter_context::encounter_desc{
		  "testing:TestMonsterPoison.chai",
		  2,
		  {100, 100, 100},
		  std::pair(1u, 10u)},
  };

  // seed used 42
  std::shared_ptr<std::mt19937> mt = std::make_shared<std::mt19937>(42);
  fseamMock->dupeReturn<FSeam::random_generator::get>(mt);

  SECTION("InGame fighting Check Character id 3 (index 2) dies because of poison") {

	fighting_pit_announcer fpa(cml);
	fpa.set_creator_user_name("Loser");
	fpa.set_creator_user_token("LoserToken");
	auto partyTeam = getPartyTeam("Loser");
	partyTeam->access_team_members().at(0)->access_status().magic_pt.current = 0;
	partyTeam->access_team_members().at(1)->access_status().magic_pt.current = 0;
	partyTeam->access_team_members().at(2)->access_status().magic_pt.current = 0;
	partyTeam->access_team_members().at(2)->access_status().life_pt.current = 99;// set to the lowest life in order to ensure as target
	partyTeam->access_team_members().at(3)->access_status().magic_pt.current = 0;
	fpa.set_creator_team_party(std::move(partyTeam));
	fpa.set_difficulty(fighting_pit::level::EASY);
	fpa.set_encounter_type(fighting_pit_announcer::encounter_type::RANDOM);

	fpa.add_action_to_one_member(0u, "arena:actions:misc:meditate.chai", 1u);// meditate level 1 restore 10 mp
	fpa.add_action_to_one_member(1u, "arena:actions:misc:meditate.chai", 2u);// meditate level 2 restore 20 mp
	fpa.add_action_to_one_member(2u, "arena:actions:misc:meditate.chai", 3u);// meditate level 3 restore 30 mp
	fpa.add_action_to_one_member(3u, "arena:actions:misc:meditate.chai", 4u);// meditate level 4 restore 40 mp

	auto fp = fpa.build_fighting_pit(ctx, "WS00");

	auto& chai = fp->get_chai_ptr();

	REQUIRE(1 == fighting_pit_announcer::get_pit_contenders(fp).number_contender());
	const auto& contender = fighting_pit_announcer::get_pit_contenders(fp).contenders().at(0);
	const auto& c = fighting_pit_announcer::get_pit_contenders(fp).contenders_on_side(hexagon_side::orientation::B_S);
	const auto& p = fighting_pit_announcer::get_party_teams(fp).members_by_side(hexagon_side::orientation::B_S);

	SECTION("Initial setup") {
	  REQUIRE("TestMonsterPoison" == contender->name());
	  REQUIRE(4 == fp->get_party_team_of_player("Loser").team_members().size());
	  REQUIRE(100 == fp->get_party_team_of_player("Loser").team_members()[0]->status().life_pt.current);
	  REQUIRE(100 == fp->get_party_team_of_player("Loser").team_members()[0]->status().life_pt.total);
	  REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);
	  REQUIRE(200 == fp->get_party_team_of_player("Loser").team_members()[1]->status().life_pt.current);
	  REQUIRE(200 == fp->get_party_team_of_player("Loser").team_members()[1]->status().life_pt.total);
	  REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);
	  REQUIRE(99 == fp->get_party_team_of_player("Loser").team_members()[2]->status().life_pt.current);// <<< LOWEST LIFE TARGET
	  REQUIRE(550 == fp->get_party_team_of_player("Loser").team_members()[2]->status().life_pt.total);
	  REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);
	  REQUIRE(140 == fp->get_party_team_of_player("Loser").team_members()[3]->status().life_pt.current);
	  REQUIRE(140 == fp->get_party_team_of_player("Loser").team_members()[3]->status().life_pt.total);
	  REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);

	  REQUIRE(1 == fighting_pit_announcer::get_pit_contenders(fp).number_contender());
	  REQUIRE(1 == fighting_pit_announcer::get_pit_contenders(fp).number_contender_on_side(hexagon_side::orientation::B_S));
	  REQUIRE(4 == fighting_pit_announcer::get_party_teams(fp).number_ally());

	  const auto& doableActions =
		  fighting_pit_announcer::get_pit_contenders(fp).contenders().at(0)->get_contender_scripting()->doable_actions();
	  REQUIRE(1 == doableActions.size());
	  REQUIRE("test:key:1" == doableActions.at(0));

	  REQUIRE(8 == c.at(0)->status().initial_speed);
	  REQUIRE(3 == p.at(0)->status().initial_speed);
	  REQUIRE(5 == p.at(1)->status().initial_speed);
	  REQUIRE(10 == p.at(2)->status().initial_speed);
	  REQUIRE(20 == p.at(3)->status().initial_speed);
	}// End section : Initial setup

	REQUIRE(fp->is_joinable());
	fp->set_player_readiness("Loser");
	REQUIRE_FALSE(fp->is_joinable());
	REQUIRE(fighting_pit_announcer::is_on_going(fp));

	// other turns are tested in the test  Test for FightingPit testcase in file PriorityOrderListTestCase.cpp

	SECTION("Battle Turn 1 test") {

	  p.at(0)->add_pending_action("meditate", std::nullopt);
	  p.at(1)->add_pending_action("meditate", std::nullopt);
	  p.at(2)->add_pending_action("meditate", std::nullopt);
	  p.at(3)->add_pending_action("meditate", std::nullopt);

	  auto now = std::chrono::system_clock::now();

	  try {
		// Player id 4 (at index 3) is going to meditate, restore 40 mp, from 0 mp to 10 (max mp 10)
		fp->continue_battle(now);
		REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[3]->status().magic_pt.current);
		REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[2]->status().magic_pt.current);
		REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[1]->status().magic_pt.current);
		REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);

		// Player id 3 (at index 2) is going to meditate, restore 30 mp, from 0 mp to 10 (max mp 10)
		now += fys::arena::interval::EASY + 1ms;
		fp->continue_battle(now);
		REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[2]->status().magic_pt.current);
		REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[1]->status().magic_pt.current);
		REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);

		// Contender turn : Target lowest life opponent:
		// ally id 1  =  100
		// ally id 2  =  200
		// ally id 3  =  99  << Target -> add poison
		// ally id 4  =  140
		now += fys::arena::interval::EASY + 1ms;
		fp->continue_battle(now);
		REQUIRE(1 == fp->get_party_team_of_player("Loser").team_members()[2]->status().alteration_before.size());
		REQUIRE("test:alterations:MortalPoison" == fp->get_party_team_of_player("Loser").team_members()[2]->status().alteration_before.at(0).alteration_key());
		REQUIRE(2 == fp->get_party_team_of_player("Loser").team_members()[2]->status().alteration_before.at(0).turn());
		REQUIRE_FALSE(fp->get_party_team_of_player("Loser").team_members()[2]->status().life_pt.is_dead());
		// Stay same
		REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[0]->status().alteration_before.size());
		REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[1]->status().alteration_before.size());
		REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[3]->status().alteration_before.size());

		// Player id 2 (at index 1) is going to meditate, restore 20 mp, from 0 mp to 0 (max mp 0)
		now += fys::arena::interval::EASY + 1ms;
		fp->continue_battle(now);
		REQUIRE_FALSE(fp->get_party_team_of_player("Loser").team_members()[2]->status().life_pt.is_dead());
		REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[1]->status().magic_pt.current);
		REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);

		// Player id 1 (at index 0) is going to meditate, restore 10 mp, from 0 mp to 10 (max mp 20)
		now += fys::arena::interval::EASY + 1ms;
		REQUIRE(fighting_pit_announcer::is_on_going(fp));
		fp->continue_battle(now);
		REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);

		SECTION("Battle Turn 2 test") {// a4, a4, a3, c0, a2, c0, a1
		  p.at(1)->add_pending_action("meditate", std::nullopt);
		  p.at(2)->add_pending_action("meditate", std::nullopt);
		  p.at(3)->add_pending_action("meditate", std::nullopt);

		  // Player id 4 (at index 3) is going to meditate, restore 20 mp, from 10 mp to 10 (max mp 10)
		  now += fys::arena::interval::EASY + 1ms;
		  fp->continue_battle(now);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[3]->status().magic_pt.current);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[2]->status().magic_pt.current);
		  REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[1]->status().magic_pt.current);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);

		  // Player id 4 (at index 3) is going to meditate, restore 20 mp, from 10 mp to 10 (max mp 10)
		  now += fys::arena::interval::EASY + 1ms;
		  fp->continue_battle(now);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[3]->status().magic_pt.current);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[2]->status().magic_pt.current);
		  REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[1]->status().magic_pt.current);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);

		  // Player id 3 (at index 2) is going to meditate, restore 30 mp, from 10 mp to 10 (max mp 10)
		  now += fys::arena::interval::EASY + 1ms;
		  fp->continue_battle(now);
		  REQUIRE(1 == fp->get_party_team_of_player("Loser").team_members()[2]->status().alteration_before.size());
		  REQUIRE("test:alterations:MortalPoison" == fp->get_party_team_of_player("Loser").team_members()[2]->status().alteration_before.at(0).alteration_key());
		  REQUIRE(1 == fp->get_party_team_of_player("Loser").team_members()[2]->status().alteration_before.at(0).turn());

		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[3]->status().magic_pt.current);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[2]->status().magic_pt.current);
		  REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[1]->status().magic_pt.current);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);

		  // Contender turn : Target lowest life opponent:
		  // ally id 1  =  100
		  // ally id 2  =  200
		  // ally id 3  =  99   << Target -> add poison (but poison already there so nothing happens)
		  // ally id 4  =  140
		  now += fys::arena::interval::EASY + 1ms;
		  fp->continue_battle(now);
		  REQUIRE(1 == fp->get_party_team_of_player("Loser").team_members()[2]->status().alteration_before.size());
		  REQUIRE("test:alterations:MortalPoison" == fp->get_party_team_of_player("Loser").team_members()[2]->status().alteration_before.at(0).alteration_key());
		  // no re-apply of the poison
		  REQUIRE(1 == fp->get_party_team_of_player("Loser").team_members()[2]->status().alteration_before.at(0).turn());
		  REQUIRE_FALSE(fp->get_party_team_of_player("Loser").team_members()[2]->status().life_pt.is_dead());
		  // Stay same
		  REQUIRE(100 == fp->get_party_team_of_player("Loser").team_members()[0]->status().life_pt.current);// damage inflicted
		  REQUIRE(100 == fp->get_party_team_of_player("Loser").team_members()[0]->status().life_pt.total);
		  REQUIRE(200 == fp->get_party_team_of_player("Loser").team_members()[1]->status().life_pt.current);
		  REQUIRE(200 == fp->get_party_team_of_player("Loser").team_members()[1]->status().life_pt.total);
		  REQUIRE(550 == fp->get_party_team_of_player("Loser").team_members()[2]->status().life_pt.total);
		  REQUIRE(140 == fp->get_party_team_of_player("Loser").team_members()[3]->status().life_pt.current);
		  REQUIRE(140 == fp->get_party_team_of_player("Loser").team_members()[3]->status().life_pt.total);
		  // Player died and is not in game anymore, he won't play next turn

		  // Player id 2 (at index 1) is going to meditate, restore 10 mp, from 0 mp to 0 (max mp 0)
		  now += fys::arena::interval::EASY + 1ms;
		  fp->continue_battle(now);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[3]->status().magic_pt.current);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[2]->status().magic_pt.current);
		  REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[1]->status().magic_pt.current);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);

		  // Contender turn : Target lowest life opponent:
		  // ally id 1  =  100 << add poison
		  // ally id 2  =  200
		  // ally id 3  =  0 (DEAD)
		  // ally id 4  =  140
		  now += fys::arena::interval::EASY + 1ms;
		  fp->continue_battle(now);
		  REQUIRE(1 == fp->get_party_team_of_player("Loser").team_members()[2]->status().alteration_before.size());
		  REQUIRE("test:alterations:MortalPoison"
				  == fp->get_party_team_of_player("Loser").team_members()[2]->status().alteration_before.at(0).alteration_key());
		  REQUIRE(1 == fp->get_party_team_of_player("Loser").team_members()[2]->status().alteration_before.at(0).turn());
		  REQUIRE_FALSE(fp->get_party_team_of_player("Loser").team_members()[2]->status().life_pt.is_dead());
		  // Stay same
		  REQUIRE(200 == fp->get_party_team_of_player("Loser").team_members()[1]->status().life_pt.current);
		  REQUIRE(200 == fp->get_party_team_of_player("Loser").team_members()[1]->status().life_pt.total);
		  REQUIRE(550 == fp->get_party_team_of_player("Loser").team_members()[2]->status().life_pt.total);
		  REQUIRE(140 == fp->get_party_team_of_player("Loser").team_members()[3]->status().life_pt.current);
		  REQUIRE(140 == fp->get_party_team_of_player("Loser").team_members()[3]->status().life_pt.total);

		  // Player id 1 (at index 0) is going to meditate, restore 10 mp, from 10 mp to 10 (max mp 10)
		  now += fys::arena::interval::EASY + 1ms;
		  fp->continue_battle(now);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[3]->status().magic_pt.current);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[2]->status().magic_pt.current);
		  REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[1]->status().magic_pt.current);
		  REQUIRE(10 == fp->get_party_team_of_player("Loser").team_members()[0]->status().magic_pt.current);

		  SECTION("Battle turn 3 test") {
			p.at(0)->add_pending_action("meditate", std::nullopt);
			p.at(2)->add_pending_action("meditate", std::nullopt);
			p.at(3)->add_pending_action("meditate", std::nullopt);

			// Player id 4 (at index 3)
			now += fys::arena::interval::EASY + 1ms;
			fp->continue_battle(now);

			REQUIRE_FALSE(fp->get_party_team_of_player("Loser").team_members()[2]->status().life_pt.is_dead());
			REQUIRE(1 == fp->get_party_team_of_player("Loser").team_members()[2]->status().alteration_before.size());
			REQUIRE("test:alterations:MortalPoison"
					== fp->get_party_team_of_player("Loser").team_members()[2]->status().alteration_before.at(0).alteration_key());
			REQUIRE(1 == fp->get_party_team_of_player("Loser").team_members()[2]->status().alteration_before.at(0).turn());

			// Player id 3 (at index 2) final turn of poison, die now
			now += fys::arena::interval::EASY + 1ms;
			fp->continue_battle(now);

			REQUIRE(fp->get_party_team_of_player("Loser").team_members()[2]->status().life_pt.is_dead());
			REQUIRE(0 == fp->get_party_team_of_player("Loser").team_members()[2]->status().alteration_before.size());

		  }// End section : Battle turn 3 test

		}// End section : Battle Turn 2 test

	  } catch (const std::exception& e) {
		SPDLOG_ERROR("Exception during Battle turn 1 test {} ", e.what());
		FAIL("Should not arrive here");
	  }

	}// End section : Battle Turn 1 test

  }// End section : InGame fighting

  // cleanup
  FSeam::MockVerifier::cleanUp();

}// End TestCase : FightingPit Simple Alteration test

#pragma clang diagnostic pop