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

#include <catch2/catch_test_macros.hpp>

#include <fightingPit/contender/contender_scripting.hh>
#include <fightingPit/contender/fighting_contender.hh>
#include <fightingPit/fighting_pit.hh>
#include <fightingPit/fighting_pit_announcer.hh>
#include <fightingPit/team/ally_party_teams.hh>
#include <fightingPit/team/party_team.hh>
#include <fightingPit/team/team_member.hh>

#include <FightingPitState_generated.h>

#include "test_type.hh"
#include <arena_server_context.hh>
#include <util/fb_utility.hh>

#include <ReplyFrame_generated.h>
#include <flatbuffer_generator.hh>

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

TEST_CASE("arn::FlatbufferGeneratorTestCase", "[service][arena][util]") {
  // Instantiate FightingPit
  auto fseamMock = FSeam::getDefault<fys::util::random_generator>();
  fys::common::connection_handler handler{};
  auto fseamConnectionHandlerMock = FSeam::get(&handler);
  auto cml = CmlBase(local_path_storage());
  encounter_context ctx;

  ctx.range_encounter_per_zone["WS00"] = {
      std::pair(4, 4),// ez
      std::pair(3, 3),// medium
      std::pair(1, 1) // hard
  };
  ctx.contenders_per_zone["WS00"] = {
      encounter_context::encounter_desc{
          "arena:contenders:Sampy.chai",
          3,
          {100, 100, 100},
          std::pair(1u, 10u)},
  };

  // seed used 42
  std::shared_ptr<std::mt19937> mt = std::make_shared<std::mt19937>(42);
  fseamMock->dupeReturn<FSeam::random_generator::get>(mt);

  fighting_pit_announcer fpa(cml);
  fpa.set_creator_user_name("Winner");
  fpa.set_creator_user_token("WinnerToken");
  fpa.set_creator_team_party(getPartyTeam("Winner"));
  fpa.set_difficulty(fighting_pit::level::EASY);
  fpa.set_encounter_type(fighting_pit_announcer::encounter_type::RANDOM);

  fpa.add_action_to_one_member(0u, "arena:actions:damage:slash_test.chai", 1u);
  fpa.add_action_to_one_member(1u, "arena:actions:damage:slash_magic.chai", 2u);
  fpa.add_action_to_one_member(2u, "arena:actions:damage:slash_demon.chai", 3u);
  fpa.add_action_to_one_member(3u, "arena:actions:damage:slash_super.chai", 4u);
  fpa.add_action_to_one_member(3u, "arena:actions:damage:slash_didi.chai", 2u);

  auto fp = fpa.build_fighting_pit(ctx, "WS00");

  REQUIRE(nullptr != fp);

  SECTION("Initial setup test") {
    REQUIRE(4 == fighting_pit_announcer::get_pit_contenders(fp).number_contender());
    auto contender1 = fighting_pit_announcer::get_pit_contenders(fp).get_contenders().at(0);
    REQUIRE("Sampy" == contender1->name());
    REQUIRE(0 == contender1->get_contender_scripting()->contender_id());
    REQUIRE(10 == contender1->get_contender_scripting()->level());
    REQUIRE(180 == contender1->status().life_pt.current);
    REQUIRE(180 == contender1->status().life_pt.total);
    REQUIRE(100 == contender1->status().magic_pt.current);
    REQUIRE(100 == contender1->status().magic_pt.total);

    auto contender2 = fighting_pit_announcer::get_pit_contenders(fp).get_contenders().at(1);
    REQUIRE("Sampy" == contender2->name());
    REQUIRE(1 == contender2->get_contender_scripting()->contender_id());
    REQUIRE(8 == contender2->get_contender_scripting()->level());
    REQUIRE(174 == contender2->status().life_pt.current);
    REQUIRE(174 == contender2->status().life_pt.total);
    REQUIRE(100 == contender2->status().magic_pt.current);
    REQUIRE(100 == contender2->status().magic_pt.total);

    auto contender3 = fighting_pit_announcer::get_pit_contenders(fp).get_contenders().at(2);
    REQUIRE("Sampy" == contender3->name());
    REQUIRE(2 == contender3->get_contender_scripting()->contender_id());
    REQUIRE(6 == contender3->get_contender_scripting()->level());
    REQUIRE(168 == contender3->status().life_pt.current);
    REQUIRE(168 == contender3->status().life_pt.total);
    REQUIRE(100 == contender3->status().magic_pt.current);
    REQUIRE(100 == contender3->status().magic_pt.total);

    auto contender4 = fighting_pit_announcer::get_pit_contenders(fp).get_contenders().at(3);
    REQUIRE(3 == contender4->get_contender_scripting()->contender_id());
    REQUIRE("Sampy" == contender4->name());
    REQUIRE(2 == contender4->get_contender_scripting()->level());
    REQUIRE(156 == contender4->status().life_pt.current);
    REQUIRE(156 == contender4->status().life_pt.total);
    REQUIRE(100 == contender4->status().magic_pt.current);
    REQUIRE(100 == contender4->status().magic_pt.total);

    REQUIRE(4 == fp->ally_party().number_ally());
    REQUIRE(4 == fp->ally_party().get_party_teams().at(0)->team_members().size());
    REQUIRE(4 == fp->ally_party().get_party_teams().at(0)->team_members().at(3)->id());
    REQUIRE(3 == fp->ally_party().get_party_teams().at(0)->team_members().at(2)->id());
    REQUIRE(2 == fp->ally_party().get_party_teams().at(0)->team_members().at(1)->id());
    REQUIRE(1 == fp->ally_party().get_party_teams().at(0)->team_members().at(0)->id());

    REQUIRE(0 == fp->id());

  }// End section : Initial setup test

  fys::arena::flatbuffer_generator fg;

  SECTION("Test generateFightingPitState") {

    auto [data, size] = fg.generateFightingPitState(*fp);

    REQUIRE(nullptr != data);
    REQUIRE(0 < size);

    const fys::fb::arn::ReplyFrame* rp = fys::fb::arn::GetReplyFrame(data);
    REQUIRE(fys::fb::arn::Content_FightingPitState == rp->content_type());
    const fys::fb::arn::FightingPitState* fps = rp->content_as_FightingPitState();

    REQUIRE(nullptr != fps);
    REQUIRE(0 == fps->idFightingPit());
    REQUIRE(1 == fps->partyTeams()->size());

    REQUIRE(4 == fps->partyTeams()->Get(0)->members()->size());
    REQUIRE("Elvo" == fps->partyTeams()->Get(0)->members()->Get(0)->characterName()->str());
    REQUIRE(1 == fps->partyTeams()->Get(0)->members()->Get(0)->status()->character_id());
    REQUIRE(fps->partyTeams()->Get(0)->members()->Get(0)->status()->isContender());
    REQUIRE(100 == fps->partyTeams()->Get(0)->members()->Get(0)->status()->current_life());
    REQUIRE(100 == fps->partyTeams()->Get(0)->members()->Get(0)->status()->total_life());
    REQUIRE(20 == fps->partyTeams()->Get(0)->members()->Get(0)->status()->current_mp());
    REQUIRE(20 == fps->partyTeams()->Get(0)->members()->Get(0)->status()->total_mp());
    REQUIRE("Mirael" == fps->partyTeams()->Get(0)->members()->Get(1)->characterName()->str());
    REQUIRE(2 == fps->partyTeams()->Get(0)->members()->Get(1)->status()->character_id());
    REQUIRE(fps->partyTeams()->Get(0)->members()->Get(1)->status()->isContender());
    REQUIRE(200 == fps->partyTeams()->Get(0)->members()->Get(1)->status()->current_life());
    REQUIRE(200 == fps->partyTeams()->Get(0)->members()->Get(1)->status()->total_life());
    REQUIRE(0 == fps->partyTeams()->Get(0)->members()->Get(1)->status()->current_mp());
    REQUIRE(0 == fps->partyTeams()->Get(0)->members()->Get(1)->status()->total_mp());
    REQUIRE("Fyston" == fps->partyTeams()->Get(0)->members()->Get(2)->characterName()->str());
    REQUIRE(3 == fps->partyTeams()->Get(0)->members()->Get(2)->status()->character_id());
    REQUIRE(fps->partyTeams()->Get(0)->members()->Get(2)->status()->isContender());
    REQUIRE(550 == fps->partyTeams()->Get(0)->members()->Get(2)->status()->current_life());
    REQUIRE(550 == fps->partyTeams()->Get(0)->members()->Get(2)->status()->total_life());
    REQUIRE(10 == fps->partyTeams()->Get(0)->members()->Get(2)->status()->current_mp());
    REQUIRE(10 == fps->partyTeams()->Get(0)->members()->Get(2)->status()->total_mp());
    REQUIRE("Simon" == fps->partyTeams()->Get(0)->members()->Get(3)->characterName()->str());
    REQUIRE(4 == fps->partyTeams()->Get(0)->members()->Get(3)->status()->character_id());
    REQUIRE(fps->partyTeams()->Get(0)->members()->Get(3)->status()->isContender());
    REQUIRE(140 == fps->partyTeams()->Get(0)->members()->Get(3)->status()->current_life());
    REQUIRE(140 == fps->partyTeams()->Get(0)->members()->Get(3)->status()->total_life());
    REQUIRE(10 == fps->partyTeams()->Get(0)->members()->Get(3)->status()->current_mp());
    REQUIRE(10 == fps->partyTeams()->Get(0)->members()->Get(3)->status()->total_mp());

    REQUIRE(5 == fps->partyTeams()->Get(0)->attacks()->size());
    REQUIRE("arena:actions:damage:slash_test.chai" == fps->partyTeams()->Get(0)->attacks()->Get(0)->str());
    REQUIRE("arena:actions:damage:slash_magic.chai" == fps->partyTeams()->Get(0)->attacks()->Get(1)->str());
    REQUIRE("arena:actions:damage:slash_demon.chai" == fps->partyTeams()->Get(0)->attacks()->Get(2)->str());
    REQUIRE("arena:actions:damage:slash_super.chai" == fps->partyTeams()->Get(0)->attacks()->Get(3)->str());
    REQUIRE("arena:actions:damage:slash_didi.chai" == fps->partyTeams()->Get(0)->attacks()->Get(4)->str());

    REQUIRE(4 == fps->monstersStatus()->size());
    REQUIRE(0 == fps->monstersStatus()->Get(0)->status()->character_id());
    REQUIRE("Sampy" == fps->monstersStatus()->Get(0)->characterName()->str());
    REQUIRE(fps->monstersStatus()->Get(0)->status()->isContender());
    REQUIRE(180 == fps->monstersStatus()->Get(0)->status()->current_life());
    REQUIRE(180 == fps->monstersStatus()->Get(0)->status()->total_life());
    REQUIRE(100 == fps->monstersStatus()->Get(0)->status()->current_mp());
    REQUIRE(100 == fps->monstersStatus()->Get(0)->status()->total_mp());

    REQUIRE(1 == fps->monstersStatus()->Get(1)->status()->character_id());
    REQUIRE("Sampy" == fps->monstersStatus()->Get(1)->characterName()->str());
    REQUIRE(fps->monstersStatus()->Get(1)->status()->isContender());
    REQUIRE(174 == fps->monstersStatus()->Get(1)->status()->current_life());
    REQUIRE(174 == fps->monstersStatus()->Get(1)->status()->total_life());
    REQUIRE(100 == fps->monstersStatus()->Get(1)->status()->current_mp());
    REQUIRE(100 == fps->monstersStatus()->Get(1)->status()->total_mp());

    REQUIRE(2 == fps->monstersStatus()->Get(2)->status()->character_id());
    REQUIRE("Sampy" == fps->monstersStatus()->Get(2)->characterName()->str());
    REQUIRE(fps->monstersStatus()->Get(2)->status()->isContender());
    REQUIRE(168 == fps->monstersStatus()->Get(2)->status()->current_life());
    REQUIRE(168 == fps->monstersStatus()->Get(2)->status()->total_life());
    REQUIRE(100 == fps->monstersStatus()->Get(2)->status()->current_mp());
    REQUIRE(100 == fps->monstersStatus()->Get(2)->status()->total_mp());

    REQUIRE(3 == fps->monstersStatus()->Get(3)->status()->character_id());
    REQUIRE("Sampy" == fps->monstersStatus()->Get(3)->characterName()->str());
    REQUIRE(fps->monstersStatus()->Get(3)->status()->isContender());
    REQUIRE(156 == fps->monstersStatus()->Get(3)->status()->current_life());
    REQUIRE(156 == fps->monstersStatus()->Get(3)->status()->total_life());
    REQUIRE(100 == fps->monstersStatus()->Get(3)->status()->current_mp());
    REQUIRE(100 == fps->monstersStatus()->Get(3)->status()->total_mp());

  }// End section : Test generateFightingPitState

  SECTION("Test generatePartyTeamStatus") {
    auto [data, size] = fg.generate_party_team_status(*fp->ally_party().get_party_teams().at(0));

    REQUIRE(nullptr != data);
    REQUIRE(0 < size);

    const fys::fb::arn::ReplyFrame* rp = fys::fb::arn::GetReplyFrame(data);
    REQUIRE(fys::fb::arn::Content_PartyTeamStatus == rp->content_type());
    const fys::fb::arn::PartyTeamStatus* pts = rp->content_as_PartyTeamStatus();

    REQUIRE(nullptr != pts);

    REQUIRE(4 == pts->members()->size());
    REQUIRE("Elvo" == pts->members()->Get(0)->characterName()->str());
    REQUIRE(1 == pts->members()->Get(0)->status()->character_id());
    REQUIRE(100 == pts->members()->Get(0)->status()->current_life());
    REQUIRE(100 == pts->members()->Get(0)->status()->total_life());
    REQUIRE(20 == pts->members()->Get(0)->status()->current_mp());
    REQUIRE(20 == pts->members()->Get(0)->status()->total_mp());
    REQUIRE("Mirael" == pts->members()->Get(1)->characterName()->str());
    REQUIRE(2 == pts->members()->Get(1)->status()->character_id());
    REQUIRE(200 == pts->members()->Get(1)->status()->current_life());
    REQUIRE(200 == pts->members()->Get(1)->status()->total_life());
    REQUIRE(0 == pts->members()->Get(1)->status()->current_mp());
    REQUIRE(0 == pts->members()->Get(1)->status()->total_mp());
    REQUIRE("Fyston" == pts->members()->Get(2)->characterName()->str());
    REQUIRE(3 == pts->members()->Get(2)->status()->character_id());
    REQUIRE(550 == pts->members()->Get(2)->status()->current_life());
    REQUIRE(550 == pts->members()->Get(2)->status()->total_life());
    REQUIRE(10 == pts->members()->Get(2)->status()->current_mp());
    REQUIRE(10 == pts->members()->Get(2)->status()->total_mp());
    REQUIRE("Simon" == pts->members()->Get(3)->characterName()->str());
    REQUIRE(4 == pts->members()->Get(3)->status()->character_id());
    REQUIRE(140 == pts->members()->Get(3)->status()->current_life());
    REQUIRE(140 == pts->members()->Get(3)->status()->total_life());
    REQUIRE(10 == pts->members()->Get(3)->status()->current_mp());
    REQUIRE(10 == pts->members()->Get(3)->status()->total_mp());

    REQUIRE(5 == pts->attacks()->size());
    REQUIRE("arena:actions:damage:slash_test.chai" == pts->attacks()->Get(0)->str());
    REQUIRE("arena:actions:damage:slash_magic.chai" == pts->attacks()->Get(1)->str());
    REQUIRE("arena:actions:damage:slash_demon.chai" == pts->attacks()->Get(2)->str());
    REQUIRE("arena:actions:damage:slash_super.chai" == pts->attacks()->Get(3)->str());
    REQUIRE("arena:actions:damage:slash_didi.chai" == pts->attacks()->Get(4)->str());

  }// End section : Test generatePartyTeamStatus

  SECTION("Test generateErrorSaturated") {
    auto [data, size] = fg.generateErrorSaturated("WS0X0");

    REQUIRE(nullptr != data);
    REQUIRE(0 < size);

    const fys::fb::arn::ReplyFrame* rp = fys::fb::arn::GetReplyFrame(data);
    REQUIRE(fys::fb::arn::Content_ErrorMessage == rp->content_type());
    const auto* pts = rp->content_as_ErrorMessage();

    REQUIRE(nullptr != pts);
    REQUIRE("Arena server is saturated: Code:WS0X0" == rp->content_as_ErrorMessage()->str());

  }// End section : Test generateErrorSaturated

  SECTION("Test generateActionNotification") {
    auto [data, size] = fg.generate_action_notification("ActionKey:Test",
                                                        fighting_pit_announcer::get_pit_contenders(fp).get_contenders(),
                                                        fp->ally_party().get_party_teams().at(0)->team_members());

    REQUIRE(nullptr != data);
    REQUIRE(0 < size);

    const fys::fb::arn::ReplyFrame* rp = fys::fb::arn::GetReplyFrame(data);
    REQUIRE(fys::fb::arn::Content_ActionExecuted == rp->content_type());
    const auto* ae = rp->content_as_ActionExecuted();

    REQUIRE(nullptr != ae);

    REQUIRE("ActionKey:Test" == ae->action()->str());

    REQUIRE(4 == ae->partyTargets()->size());
    REQUIRE("Elvo" == ae->partyTargets()->Get(0)->characterName()->str());
    REQUIRE("Mirael" == ae->partyTargets()->Get(1)->characterName()->str());
    REQUIRE("Fyston" == ae->partyTargets()->Get(2)->characterName()->str());
    REQUIRE("Simon" == ae->partyTargets()->Get(3)->characterName()->str());

    REQUIRE(4 == ae->monstersTargets()->size());
    REQUIRE(0 == ae->monstersTargets()->Get(0)->status()->character_id());
    REQUIRE("Sampy" == ae->monstersTargets()->Get(0)->characterName()->str());
    REQUIRE(1 == ae->monstersTargets()->Get(1)->status()->character_id());
    REQUIRE("Sampy" == ae->monstersTargets()->Get(1)->characterName()->str());
    REQUIRE(2 == ae->monstersTargets()->Get(2)->status()->character_id());
    REQUIRE("Sampy" == ae->monstersTargets()->Get(2)->characterName()->str());
    REQUIRE(3 == ae->monstersTargets()->Get(3)->status()->character_id());
    REQUIRE("Sampy" == ae->monstersTargets()->Get(3)->characterName()->str());

  }// End section : Test generateActionNotification

  //	SECTION("Test Generate Cosmetic") {
  //
  //	} // End section : Test Generate Cosmetic

}// End TestCase : Flatbuffer Generator Test