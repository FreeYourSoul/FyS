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

#include "TestType.hh"
#include <FSeamMockData.hpp>
#include <arena_server_context.hh>
#include <catch2/catch.hpp>
#include <fightingPit/contender/contender_scripting.hh>
#include <fightingPit/contender/fighting_contender.hh>
#include <fightingPit/data/common_types.hh>
#include <fightingPit/fighting_pit_announcer.hh>
#include <random_generator.hh>

using namespace fys::arena;

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

TEST_CASE("FightingPitAnnouncerTestCase", "[service][arena]") {
  auto fseamMock = FSeam::getDefault<fys::util::random_generator>();
  auto cml = CmlBase(local_path_storage());
  encounter_context ctx;
  ctx.range_encounter_per_zone["WS00"] = {
      encounter_context::rng_range(1, 4),// ez
      encounter_context::rng_range(2, 4),// medium
      encounter_context::rng_range(3, 5) // hard
  };
  ctx.contenders_per_zone["WS00"] = {
      encounter_context::encounter_desc{
          "arena:contenders:Sampy.chai", 3,
          encounter_context::chance_array{60, 60, 60},
          encounter_context::rng_range(1u, 10u)},
      encounter_context::encounter_desc{
          "arena:contenders:Slime.chai", 3,
          encounter_context::chance_array{40, 40, 40},
          encounter_context::rng_range(1u, 10u)}};

  SECTION("test wrong range encounter") {
    fighting_pit_announcer fpa(cml);
    fpa.set_creator_user_token(" ");
    fpa.set_creator_user_name(" ");
    fpa.set_creator_team_party(getPartyTeam(" "));
    fpa.set_difficulty(fighting_pit::MEDIUM);
    fpa.set_encounter_type(fighting_pit_announcer::encounter_type::RANDOM);
    REQUIRE(nullptr == fpa.build_fighting_pit(ctx, "WS42"));
  }// End section : test wrong range encounter

  SECTION("test no creator userName") {
    fighting_pit_announcer fpa(cml);
    fpa.set_creator_user_token(" ");
    // fpa.setCreatorUserName(); !!NOT SET
    fpa.set_creator_team_party(getPartyTeam(" "));
    fpa.set_difficulty(fighting_pit::MEDIUM);
    fpa.set_encounter_type(fighting_pit_announcer::encounter_type::RANDOM);
    REQUIRE(nullptr == fpa.build_fighting_pit(ctx, "WS00"));
  }// End section : test no creator userName

  SECTION("test no creator userToken") {
    fighting_pit_announcer fpa(cml);
    fpa.set_creator_user_name(" ");
    // fpa.setCreatorUserToken(); !!NOT SET
    fpa.set_creator_team_party(getPartyTeam(" "));
    fpa.set_difficulty(fighting_pit::MEDIUM);
    fpa.set_encounter_type(fighting_pit_announcer::encounter_type::RANDOM);
    REQUIRE(nullptr == fpa.build_fighting_pit(ctx, "WS00"));
  }// End section : test no creator userName

  SECTION("test invalid action name registered") {

    std::shared_ptr<std::mt19937> mt = std::make_shared<std::mt19937>(42);
    fseamMock->dupeReturn<FSeam::random_generator::get>(mt);

    fighting_pit_announcer fpa(cml);
    fpa.set_creator_user_token(" ");
    fpa.set_creator_user_name(" ");
    fpa.set_creator_team_party(getPartyTeam(" "));
    fpa.set_difficulty(fighting_pit::HARD);
    fpa.set_encounter_type(fighting_pit_announcer::encounter_type::RANDOM);
    fpa.add_action_to_one_member(0, "arena", 5);
    auto fp = fpa.build_fighting_pit(ctx, "WS00");

  }// End section : test invalid action name registered

  SECTION("Test Side Setup") {
    std::shared_ptr<std::mt19937> mt = std::make_shared<std::mt19937>(42);
    fseamMock->dupeReturn<FSeam::random_generator::get>(mt);
    fighting_pit_announcer fpa(cml);
    fpa.set_creator_user_name(" ");
    fpa.set_creator_user_token(" ");
    fpa.set_creator_team_party(getPartyTeam(" "));
    fpa.set_difficulty(fighting_pit::EASY);
    fpa.set_encounter_type(fighting_pit_announcer::encounter_type::RANDOM);
    fpa.add_action_to_one_member(0, "arena:actions:damage:slash.chai", 5);
    auto fp = fpa.build_fighting_pit(ctx, "WS00");

    const auto& sides = fighting_pit_announcer::get_side_vector(fp);
    REQUIRE(18 == sides.size());

    REQUIRE(hexagon_side::orientation::A_N == sides[static_cast<uint>(hexagon_side::orientation::A_N)].side());
    REQUIRE(hexagon_side::orientation::A_NE == sides[static_cast<uint>(hexagon_side::orientation::A_NE)].side());
    REQUIRE(hexagon_side::orientation::A_NW == sides[static_cast<uint>(hexagon_side::orientation::A_NW)].side());
    REQUIRE(hexagon_side::orientation::A_S == sides[static_cast<uint>(hexagon_side::orientation::A_S)].side());
    REQUIRE(hexagon_side::orientation::A_SE == sides[static_cast<uint>(hexagon_side::orientation::A_SE)].side());
    REQUIRE(hexagon_side::orientation::A_SW == sides[static_cast<uint>(hexagon_side::orientation::A_SW)].side());

    REQUIRE(hexagon_side::orientation::B_N == sides[static_cast<uint>(hexagon_side::orientation::B_N)].side());
    REQUIRE(hexagon_side::orientation::B_NE == sides[static_cast<uint>(hexagon_side::orientation::B_NE)].side());
    REQUIRE(hexagon_side::orientation::B_NW == sides[static_cast<uint>(hexagon_side::orientation::B_NW)].side());
    REQUIRE(hexagon_side::orientation::B_S == sides[static_cast<uint>(hexagon_side::orientation::B_S)].side());
    REQUIRE(hexagon_side::orientation::B_SE == sides[static_cast<uint>(hexagon_side::orientation::B_SE)].side());
    REQUIRE(hexagon_side::orientation::B_SW == sides[static_cast<uint>(hexagon_side::orientation::B_SW)].side());

    REQUIRE(hexagon_side::orientation::C_N == sides[static_cast<uint>(hexagon_side::orientation::C_N)].side());
    REQUIRE(hexagon_side::orientation::C_NE == sides[static_cast<uint>(hexagon_side::orientation::C_NE)].side());
    REQUIRE(hexagon_side::orientation::C_NW == sides[static_cast<uint>(hexagon_side::orientation::C_NW)].side());
    REQUIRE(hexagon_side::orientation::C_S == sides[static_cast<uint>(hexagon_side::orientation::C_S)].side());
    REQUIRE(hexagon_side::orientation::C_SE == sides[static_cast<uint>(hexagon_side::orientation::C_SE)].side());
    REQUIRE(hexagon_side::orientation::C_SW == sides[static_cast<uint>(hexagon_side::orientation::C_SW)].side());
  }// End section : Test Side Setup

  SECTION("test seed 42") {
    std::shared_ptr<std::mt19937> mt = std::make_shared<std::mt19937>(42);
    fseamMock->dupeReturn<FSeam::random_generator::get>(mt);

    SECTION("test seed ez") {
      REQUIRE(2 == fys::util::random_generator::generate_in_range(1, 4));
      // encounter 1
      REQUIRE(80 == fys::util::random_generator::generate_in_range(0, 100));
      REQUIRE(10 == fys::util::random_generator::generate_in_range(1, 10));
      // encounter 2
      REQUIRE(18 == fys::util::random_generator::generate_in_range(0, 100));
      REQUIRE(8 == fys::util::random_generator::generate_in_range(1, 10));

    }// End section : test seed ez
    SECTION("test seed medium") {
      REQUIRE(3 == fys::util::random_generator::generate_in_range(2, 4));
      // encounter 1
      REQUIRE(80 == fys::util::random_generator::generate_in_range(0, 100));
      REQUIRE(10 == fys::util::random_generator::generate_in_range(1, 10));
      // encounter 2
      REQUIRE(18 == fys::util::random_generator::generate_in_range(0, 100));
      REQUIRE(8 == fys::util::random_generator::generate_in_range(1, 10));
      // encounter 3
      REQUIRE(78 == fys::util::random_generator::generate_in_range(0, 100));
      REQUIRE(6 == fys::util::random_generator::generate_in_range(1, 10));
    }// End section : test seed medium

    SECTION("test seed hard") {
      REQUIRE(4 == fys::util::random_generator::generate_in_range(3, 5));
      // encounter 1
      REQUIRE(80 == fys::util::random_generator::generate_in_range(0, 100));
      REQUIRE(10 == fys::util::random_generator::generate_in_range(1, 10));
      // encounter 2
      REQUIRE(18 == fys::util::random_generator::generate_in_range(0, 100));
      REQUIRE(8 == fys::util::random_generator::generate_in_range(1, 10));
      // encounter 3
      REQUIRE(78 == fys::util::random_generator::generate_in_range(0, 100));
      REQUIRE(6 == fys::util::random_generator::generate_in_range(1, 10));
      // encounter 4
      REQUIRE(60 == fys::util::random_generator::generate_in_range(0, 100));
      REQUIRE(2 == fys::util::random_generator::generate_in_range(1, 10));
    }// End section : test seed hard

    SECTION("test generate contender Easy") {// seed 2 80 10 28 8
      fighting_pit_announcer fpa(cml);
      fpa.set_creator_user_name(" ");
      fpa.set_creator_user_token(" ");
      fpa.set_creator_team_party(getPartyTeam(" "));
      fpa.set_difficulty(fighting_pit::EASY);
      fpa.set_encounter_type(fighting_pit_announcer::encounter_type::RANDOM);
      fpa.add_action_to_one_member(0, "arena:actions:damage:slash.chai", 5);
      auto fp = fpa.build_fighting_pit(ctx, "WS00");

      REQUIRE_FALSE(fp->is_battle_over());
      REQUIRE(fp->is_joinable());
      REQUIRE_THROWS(fp->set_player_readiness("NotExisting"));// test if the fp throws in this case

      REQUIRE(2 == fighting_pit_announcer::get_pit_contenders(fp).number_contender());

      REQUIRE("Slime" == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(0)->name());
      REQUIRE(0 == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(0)->id());
      REQUIRE(10 == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(0)->get_contender_scripting()->level());

      REQUIRE("Sampy" == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(1)->name());
      REQUIRE(1 == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(1)->get_contender_scripting()->contender_id());
      REQUIRE(8 == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(1)->get_contender_scripting()->level());

    }// End section : test generate contender Easy

    SECTION("test generate contender Medium") {// seed 3 80 10 18 8 78 6
      fighting_pit_announcer fpa(cml);
      fpa.set_creator_user_token(" ");
      fpa.set_creator_user_name(" ");
      fpa.set_creator_team_party(getPartyTeam(" "));
      fpa.set_join_disabled(true);
      fpa.set_difficulty(fighting_pit::MEDIUM);
      fpa.set_encounter_type(fighting_pit_announcer::encounter_type::RANDOM);
      fpa.add_action_to_one_member(0, "arena:actions:damage:slash.chai", 5);
      auto fp = fpa.build_fighting_pit(ctx, "WS00");

      REQUIRE(4 == fighting_pit_announcer::get_party_teams(fp).get_party_team_of_player(" ").team_members().size());
      REQUIRE_FALSE(fp->is_joinable());

      REQUIRE(3 == fighting_pit_announcer::get_pit_contenders(fp).number_contender());

      REQUIRE("Slime" == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(0)->name());
      REQUIRE(0 == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(0)->get_contender_scripting()->contender_id());
      REQUIRE(10 == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(0)->get_contender_scripting()->level());

      REQUIRE("Sampy" == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(1)->name());
      REQUIRE(1 == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(1)->get_contender_scripting()->contender_id());
      REQUIRE(8 == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(1)->get_contender_scripting()->level());

      REQUIRE("Slime" == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(2)->name());
      REQUIRE(2 == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(2)->get_contender_scripting()->contender_id());
      REQUIRE(6 == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(2)->get_contender_scripting()->level());

    }// End section : test generate contender Medium

    SECTION("test generate contender Hard") {// seed 4 80 10 18 8 78 6 60 2
      fighting_pit_announcer fpa(cml);
      fpa.set_creator_user_token(" ");
      fpa.set_creator_user_name(" ");
      fpa.set_creator_team_party(getPartyTeam(" "));
      fpa.set_difficulty(fighting_pit::HARD);
      fpa.set_encounter_type(fighting_pit_announcer::encounter_type::RANDOM);
      fpa.add_action_to_one_member(0, "arena:actions:damage:slash.chai", 5);
      auto fp = fpa.build_fighting_pit(ctx, "WS00");

      REQUIRE(4 == fighting_pit_announcer::get_pit_contenders(fp).number_contender());
      REQUIRE(fp->is_joinable());

      REQUIRE("Slime" == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(0)->name());
      REQUIRE(0 == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(0)->get_contender_scripting()->contender_id());
      REQUIRE(10 == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(0)->get_contender_scripting()->level());

      REQUIRE("Sampy" == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(1)->name());
      REQUIRE(1 == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(1)->get_contender_scripting()->contender_id());
      REQUIRE(8 == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(1)->get_contender_scripting()->level());

      REQUIRE("Slime" == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(2)->name());
      REQUIRE(2 == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(2)->get_contender_scripting()->contender_id());
      REQUIRE(6 == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(2)->get_contender_scripting()->level());

      REQUIRE("Sampy" == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(3)->name());
      REQUIRE(3 == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(3)->get_contender_scripting()->contender_id());
      REQUIRE(2 == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(3)->get_contender_scripting()->level());

    }// End section : test generate contender Hard

  }// End section : RNG test 42

  SECTION("test seed 1337") {
    std::shared_ptr<std::mt19937> mt = std::make_shared<std::mt19937>(1337);
    fseamMock->dupeReturn<FSeam::random_generator::get>(mt);

    SECTION("test seed Easy") {
      REQUIRE(2 == fys::util::random_generator::generate_in_range(1, 4));
      // encounter 1
      REQUIRE(56 == fys::util::random_generator::generate_in_range(0, 100));
      REQUIRE(2 == fys::util::random_generator::generate_in_range(1, 10));
      // encounter 2
      REQUIRE(21 == fys::util::random_generator::generate_in_range(0, 100));
      REQUIRE(3 == fys::util::random_generator::generate_in_range(1, 10));
    }// End section : test seed Easy

    SECTION("test generate contender Easy") {// seed 2 56 2 21 3
      fighting_pit_announcer fpa(cml);
      fpa.set_creator_user_token(" ");
      fpa.set_creator_user_name(" ");
      fpa.set_creator_team_party(getPartyTeam(" "));
      fpa.set_difficulty(fighting_pit::EASY);
      fpa.set_encounter_type(fighting_pit_announcer::encounter_type::RANDOM);
      fpa.add_action_to_one_member(0, "arena:actions:damage:slash.chai", 5);
      auto fp = fpa.build_fighting_pit(ctx, "WS00");

      REQUIRE(2 == fighting_pit_announcer::get_pit_contenders(fp).number_contender());

      REQUIRE("Sampy" == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(0)->name());
      REQUIRE(0 == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(0)->get_contender_scripting()->contender_id());
      REQUIRE(2 == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(0)->get_contender_scripting()->level());

      REQUIRE("Sampy" == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(1)->name());
      REQUIRE(1 == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(1)->get_contender_scripting()->contender_id());
      REQUIRE(3 == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(1)->get_contender_scripting()->level());

      SECTION("test ordering turn setup") {
        REQUIRE(8 == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(0)->status().initial_speed);
        REQUIRE(hexagon_side::orientation::B_S == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(0)->side_orient());
        REQUIRE(8 == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(1)->status().initial_speed);
        REQUIRE(hexagon_side::orientation::B_S == fighting_pit_announcer::get_pit_contenders(fp).fighting_contender_at(1)->side_orient());

        const auto members = fighting_pit_announcer::get_party_teams(fp).members_by_side(hexagon_side::orientation::B_S);
        REQUIRE(4 == members.size());
        REQUIRE(1 == members.at(0)->id());
        REQUIRE(3 == members.at(0)->status().initial_speed);
        REQUIRE(2 == members.at(1)->id());
        REQUIRE(5 == members.at(1)->status().initial_speed);
        REQUIRE(3 == members.at(2)->id());
        REQUIRE(10 == members.at(2)->status().initial_speed);
        REQUIRE(4 == members.at(3)->id());
        REQUIRE(20 == members.at(3)->status().initial_speed);

        SECTION("test ordering") {

          using namespace std::chrono_literals;
          auto& order = fighting_pit_announcer::get_side_battle_for_side(fp, hexagon_side::orientation::B_S);
          REQUIRE(hexagon_side::orientation::B_S == order.side());

          const auto elem1 = order.get_current_participant_turn(std::chrono::system_clock::now(), 0ms);
          REQUIRE(data::priority_elem{4, 20, data::PARTY_MEMBER} == elem1);
          REQUIRE(20 == elem1.speed);
          std::this_thread::sleep_for(10ms);

          const auto elem2 = order.get_current_participant_turn(std::chrono::system_clock::now(), 0ms);
          REQUIRE(data::priority_elem{3, 10, data::PARTY_MEMBER} == elem2);
          REQUIRE(10 == elem2.speed);
          std::this_thread::sleep_for(10ms);

          const auto elem3 = order.get_current_participant_turn(std::chrono::system_clock::now(), 0ms);
          REQUIRE(data::priority_elem{1, 8, data::CONTENDER} == elem3);
          REQUIRE(8 == elem3.speed);
          std::this_thread::sleep_for(10ms);

          const auto elem4 = order.get_current_participant_turn(std::chrono::system_clock::now(), 0ms);
          REQUIRE(data::priority_elem{0, 8, data::CONTENDER} == elem4);
          REQUIRE(8 == elem4.speed);
          std::this_thread::sleep_for(10ms);

          const auto elem5 = order.get_current_participant_turn(std::chrono::system_clock::now(), 0ms);
          REQUIRE(data::priority_elem{2, 5, data::PARTY_MEMBER} == elem5);
          REQUIRE(5 == elem5.speed);
          std::this_thread::sleep_for(10ms);

          const auto& elem = order.get_current_participant_turn(std::chrono::system_clock::now(), 0ms);
          REQUIRE(data::priority_elem{1, 4, data::PARTY_MEMBER} == elem);
          REQUIRE(3 == elem.speed);
          std::this_thread::sleep_for(10ms);

        }// End section : test ordering
      }

    }// End section : test generate contender Easy

  }// End section : RNG test 1337

  FSeam::MockVerifier::cleanUp();

}// End TestCase : FightingPitAnnouncer test

TEST_CASE("FightingPitAnnouncerTestCase test reward", "[service][arena]") {
  auto fseamMock = FSeam::getDefault<fys::util::random_generator>();
  auto cml = CmlBase(local_path_storage());
  encounter_context ctx;
  ctx.range_encounter_per_zone["WS00"] = {
      encounter_context::rng_range(1, 4),// ez
      encounter_context::rng_range(1, 4),// medium
      encounter_context::rng_range(1, 4) // hard
  };
  ctx.contenders_per_zone["WS00"] = {
      encounter_context::encounter_desc{
          "arena:contenders:Sampy.chai", 3,
          encounter_context::chance_array{60, 60, 60},
          encounter_context::rng_range(1u, 10u)},
      encounter_context::encounter_desc{
          "arena:contenders:Slime.chai", 3,
          encounter_context::chance_array{40, 40, 40},
          encounter_context::rng_range(1u, 10u)}};
  ctx.reward_desc_per_contender["Sampy"] = encounter_context::reward_encounter_desc{
      std::array<encounter_context::rng_range, 3>{
          encounter_context::rng_range(3, 3),
          encounter_context::rng_range(2, 2),
          encounter_context::rng_range(6, 6)},
      {
          {"DrPepper", encounter_context::chance_array{50, 50, 50}},
          {"EsCaliBur", encounter_context::chance_array{50, 50, 50}},
      }};
  std::shared_ptr<std::mt19937> mt = std::make_shared<std::mt19937>(42);
  fseamMock->dupeReturn<FSeam::random_generator::get>(mt);

  SECTION("test seed") {
    REQUIRE(2 == fys::util::random_generator::generate_in_range(1, 4));
    // encounter 1
    REQUIRE(80 == fys::util::random_generator::generate_in_range(0, 100));
    REQUIRE(10 == fys::util::random_generator::generate_in_range(1, 10));
    // encounter 2
    REQUIRE(18 == fys::util::random_generator::generate_in_range(0, 100));
    REQUIRE(8 == fys::util::random_generator::generate_in_range(1, 10));
    // Reward
    REQUIRE(3 == fys::util::random_generator::generate_in_range(3, 3));   // number of reward
    REQUIRE(60 == fys::util::random_generator::generate_in_range(0, 100));// first  reward is EsCaliBur
    REQUIRE(60 == fys::util::random_generator::generate_in_range(0, 100));// second reward is EsCaliBur
    REQUIRE(15 == fys::util::random_generator::generate_in_range(0, 100));// third  reward is DrPepper

  }// End section : test seed ez

  SECTION("Test one type reward generation") {
    fighting_pit_announcer fpa(cml);
    fpa.set_creator_user_name(" ");
    fpa.set_creator_user_token(" ");
    fpa.set_creator_team_party(getPartyTeam(" "));
    fpa.set_difficulty(fighting_pit::MEDIUM);
    fpa.set_encounter_type(fighting_pit_announcer::encounter_type::RANDOM);
    fpa.add_action_to_one_member(0, "arena:actions:damage:slash.chai", 5);
    auto fp = fpa.build_fighting_pit(ctx, "WS00");

    REQUIRE(1 == fighting_pit_announcer::get_reward(fp).keys.size());
    REQUIRE(1 == fighting_pit_announcer::get_reward(fp).quantity.size());
    REQUIRE("EsCaliBur" == fighting_pit_announcer::get_reward(fp).keys.at(0));
    REQUIRE(2 == fighting_pit_announcer::get_reward(fp).quantity.at(0));

  }// End section : Test one type reward generation

  SECTION("Test different reward generation") {
    fighting_pit_announcer fpa(cml);
    fpa.set_creator_user_name(" ");
    fpa.set_creator_user_token(" ");
    fpa.set_creator_team_party(getPartyTeam(" "));
    fpa.set_difficulty(fighting_pit::EASY);
    fpa.set_encounter_type(fighting_pit_announcer::encounter_type::RANDOM);
    fpa.add_action_to_one_member(0, "arena:actions:damage:slash.chai", 5);
    auto fp = fpa.build_fighting_pit(ctx, "WS00");

    REQUIRE(2 == fighting_pit_announcer::get_reward(fp).keys.size());// 2 different reward
    REQUIRE(2 == fighting_pit_announcer::get_reward(fp).quantity.size());
    REQUIRE("DrPepper" == fighting_pit_announcer::get_reward(fp).keys.at(0));
    REQUIRE(1 == fighting_pit_announcer::get_reward(fp).quantity.at(0));
    REQUIRE("EsCaliBur" == fighting_pit_announcer::get_reward(fp).keys.at(1));
    REQUIRE(2 == fighting_pit_announcer::get_reward(fp).quantity.at(1));
  }// End section : Test different reward generation

  SECTION("test seed for 'Multiple Monster of same type add up'") {
    REQUIRE(2 == fys::util::random_generator::generate_in_range(1, 4));
    // encounter 1
    REQUIRE(80 == fys::util::random_generator::generate_in_range(0, 100));// Slime is selected
    REQUIRE(10 == fys::util::random_generator::generate_in_range(1, 10)); // level Slime is 10
    // encounter 2
    REQUIRE(18 == fys::util::random_generator::generate_in_range(0, 100));// Sampy is selected
    REQUIRE(8 == fys::util::random_generator::generate_in_range(1, 10));  // level Sampy is 8

    // Reward Slime
    REQUIRE(3 == fys::util::random_generator::generate_in_range(3, 3));   // number of reward for Slime
    REQUIRE(60 == fys::util::random_generator::generate_in_range(0, 100));// first  reward is TriForce
    REQUIRE(60 == fys::util::random_generator::generate_in_range(0, 100));// second reward is TriForce
    REQUIRE(15 == fys::util::random_generator::generate_in_range(0, 100));// third  reward is TriForce
    // Reward Sampy
    REQUIRE(6 == fys::util::random_generator::generate_in_range(6, 6));   // number of reward for Sampy
    REQUIRE(15 == fys::util::random_generator::generate_in_range(0, 100));// first  reward is DrPepper
    REQUIRE(10 == fys::util::random_generator::generate_in_range(0, 100));// second reward is DrPepper
    REQUIRE(5 == fys::util::random_generator::generate_in_range(0, 100)); // third  reward is DrPepper
    REQUIRE(46 == fys::util::random_generator::generate_in_range(0, 100));// fourth reward is DrPepper
    REQUIRE(87 == fys::util::random_generator::generate_in_range(0, 100));// fith   reward is EsCaliBur
    REQUIRE(33 == fys::util::random_generator::generate_in_range(0, 100));// sixth  reward is DrPepper

  }// End section : test seed ez

  SECTION("Multiple Monster of same type add up") {

    ctx.reward_desc_per_contender["Slime"] = encounter_context::reward_encounter_desc{
        std::array<encounter_context::rng_range, 3>{
            encounter_context::rng_range(3, 3),
            encounter_context::rng_range(2, 2),
            encounter_context::rng_range(3, 3)},
        {{"TriForce", encounter_context::chance_array{100, 100, 100}}}};
    fighting_pit_announcer fpa(cml);
    fpa.set_creator_user_name(" ");
    fpa.set_creator_user_token(" ");
    fpa.set_creator_team_party(getPartyTeam(" "));
    fpa.set_difficulty(fighting_pit::HARD);
    fpa.set_encounter_type(fighting_pit_announcer::encounter_type::RANDOM);
    fpa.add_action_to_one_member(0, "arena:actions:damage:slash.chai", 5);
    auto fp = fpa.build_fighting_pit(ctx, "WS00");

    // 3 different rewards
    REQUIRE(3 == fighting_pit_announcer::get_reward(fp).keys.size());
    REQUIRE(3 == fighting_pit_announcer::get_reward(fp).quantity.size());

    REQUIRE("DrPepper" == fighting_pit_announcer::get_reward(fp).keys.at(0));
    REQUIRE(5 == fighting_pit_announcer::get_reward(fp).quantity.at(0));
    REQUIRE("EsCaliBur" == fighting_pit_announcer::get_reward(fp).keys.at(1));
    REQUIRE(1 == fighting_pit_announcer::get_reward(fp).quantity.at(1));
    REQUIRE("TriForce" == fighting_pit_announcer::get_reward(fp).keys.at(2));
    REQUIRE(3 == fighting_pit_announcer::get_reward(fp).quantity.at(2));

  }// End section : Multiple Monster of same type add up

}// End TestCase :  FightingPitAnnouncer test reward

#pragma clang diagnostic pop