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

#include <ArenaFightAction_generated.h>
#include <ArenaServerAuth_generated.h>
#include <ArenaServerValidateAuth_generated.h>
#include <EndBattle_generated.h>
#include <FightingPitEncounter_generated.h>
#include <FightingPitState_generated.h>
#include <catch2/catch.hpp>
#include <flatbuffers/flatbuffers.h>

TEST_CASE("ArenaComFlatBufferTestCase FightingPitEncounter", "[service][arena][common][fb]") {
  flatbuffers::FlatBufferBuilder fbb;
  auto fp = fys::fb::arn::CreateFightingPitEncounter(
      fbb,
      fbb.CreateString("name"),
      fbb.CreateString("token_authentication"),
      1337,
      fbb.CreateString("WS001"),
      false,
      fys::fb::arn::Level::Level_EASY,
      4242,
      false,
      0.0,
      0.0);
  fys::fb::arn::FinishFightingPitEncounterBuffer(fbb, fp);

  SECTION("Verifier") {
    auto ok = flatbuffers::Verifier(fbb.GetBufferPointer(), fbb.GetSize());
    CHECK(fys::fb::arn::VerifyFightingPitEncounterBuffer(ok));
  }
  uint8_t* binary = fbb.GetBufferPointer();

  SECTION("Binary to FlatBuffer") {
    const fys::fb::arn::FightingPitEncounter* fromBinary = fys::fb::arn::GetFightingPitEncounter(binary);
    REQUIRE("name" == fromBinary->user_name()->str());
    REQUIRE("token_authentication" == fromBinary->token_auth()->str());
    REQUIRE(4242 == fromBinary->id_encounter());
    REQUIRE_FALSE(fromBinary->is_ambush());
    REQUIRE_FALSE(fromBinary->disable_join());
    REQUIRE(1337 == fromBinary->fighting_pit_id());

  }// End section : Binary to Flatbuffer

}// End Test Case : ArenaComFlatBufferTestCase FightingPitEncounter

TEST_CASE("ArenaComFlatBufferTestCase ArenaServerAuth", "[service][arena][common][fb]") {

  flatbuffers::FlatBufferBuilder fbb;
  auto asa = fys::fb::arn::CreateArenaServerAuth(
      fbb,
      fbb.CreateString("name"),
      fbb.CreateString("token_authentication"),
      fbb.CreateString("localhost"),
      fbb.CreateString("tcp://localhost:4242"),
      42,
      fbb.CreateString("WS001"));
  fys::fb::arn::FinishArenaServerAuthBuffer(fbb, asa);

  SECTION("Verifier") {
    auto ok = flatbuffers::Verifier(fbb.GetBufferPointer(), fbb.GetSize());
    CHECK(fys::fb::arn::VerifyArenaServerAuthBuffer(ok));
  }
  uint8_t* binary = fbb.GetBufferPointer();

  SECTION("Binary to FlatBuffer") {
    const fys::fb::arn::ArenaServerAuth* fromBinary = fys::fb::arn::GetArenaServerAuth(binary);
    REQUIRE("name" == fromBinary->user_name()->str());
    REQUIRE("token_authentication" == fromBinary->token_auth()->str());
    REQUIRE("localhost" == fromBinary->ip()->str());
    REQUIRE("tcp://localhost:4242" == fromBinary->connection_str()->str());
    REQUIRE("WS001" == fromBinary->arena_server_code()->str());
    REQUIRE(42 == fromBinary->id_fightingpit());

    SECTION("Verifier") {

    }// End section : Verifier

  }// End section : Binary to Flatbuffer

}// End Test Case : ArenaComFlatBufferTestCase ArenaServerAuth

TEST_CASE("ArenaComFlatBufferTestCase ArenaServerValidateAuth", "[service][arena][common][fb]") {

  flatbuffers::FlatBufferBuilder fbb;
  auto asva = fys::fb::arn::CreateArenaServerValidateAuth(
      fbb,
      fbb.CreateString("name"),
      fbb.CreateString("token_authentication"),
      42);
  fys::fb::arn::FinishArenaServerValidateAuthBuffer(fbb, asva);

  SECTION("Verifier") {
    auto ok = flatbuffers::Verifier(fbb.GetBufferPointer(), fbb.GetSize());
    CHECK(fys::fb::arn::VerifyArenaServerValidateAuthBuffer(ok));
  }

  SECTION("Binary to FlatBuffer") {
    uint8_t* binary = fbb.GetBufferPointer();
    const fys::fb::arn::ArenaServerValidateAuth* fromBinary = fys::fb::arn::GetArenaServerValidateAuth(binary);
    REQUIRE("name" == fromBinary->user_name()->str());
    REQUIRE("token_authentication" == fromBinary->token_auth()->str());
    REQUIRE(42 == fromBinary->fighting_pit_id());

  }// End section : Binary to Flatbuffer

}// End Test Case : ArenaComFlatBufferTestCase ArenaServerValidateAuth

TEST_CASE("ArenaComFlatBufferTestCase ArenaFightAction", "[service][arena][common][fb]") {

  flatbuffers::FlatBufferBuilder fbb;
  auto afa = fys::fb::arn::CreateArenaFightAction(
      fbb,
      1337,
      fbb.CreateString("idAction"),
      fys::fb::arn::Targeting_ENNEMIES,
      fbb.CreateVector(std::vector<uint>{1, 4, 5, 6}),
      fbb.CreateVector(std::vector<uint>{}));
  fys::fb::arn::FinishArenaFightActionBuffer(fbb, afa);

  SECTION("Verifier") {
    auto ok = flatbuffers::Verifier(fbb.GetBufferPointer(), fbb.GetSize());
    CHECK(fys::fb::arn::VerifyArenaFightActionBuffer(ok));
  }

  SECTION("Binary to FlatBuffer") {
    uint8_t* binary = fbb.GetBufferPointer();
    const fys::fb::arn::ArenaFightAction* fromBinary = fys::fb::arn::GetArenaFightAction(binary);
    REQUIRE("idAction" == fromBinary->actionId()->str());
    REQUIRE(1337 == fromBinary->memberId());
    REQUIRE(fys::fb::arn::Targeting_ENNEMIES == fromBinary->targetType());
    REQUIRE(1 == fromBinary->targetId_contender()->Get(0));
    REQUIRE(4 == fromBinary->targetId_contender()->Get(1));
    REQUIRE(5 == fromBinary->targetId_contender()->Get(2));
    REQUIRE(6 == fromBinary->targetId_contender()->Get(3));

  }// End section : Binary to Flatbuffer

}// End Test Case : ArenaComFlatBufferTestCase ArenaFightAction

TEST_CASE("ArenaComFlatBufferTestCase PartyTeamStatus", "[service][arena][common][fb]") {
  // test MemberStatus
  flatbuffers::FlatBufferBuilder fbb;

  const fys::fb::arn::CharacterStatus cs = fys::fb::arn::CharacterStatus{
      0,
      false,
      90,
      100,
      190,
      200,
      fys::fb::arn::SideOrient_A_N,
  };
  auto mso = fys::fb::arn::CreateMemberStatus(fbb, fbb.CreateString("memberName1"), &cs);
  fbb.Finish(mso);

  SECTION("Verifier") {
    auto ok = flatbuffers::Verifier(fbb.GetBufferPointer(), fbb.GetSize());
    CHECK(ok.VerifyBuffer<fys::fb::arn::MemberStatus>());
  }

  uint8_t* b = fbb.GetBufferPointer();

  SECTION("TeamMember Binary to FlatBuffer") {
    const auto* fromBinary = flatbuffers::GetRoot<fys::fb::arn::MemberStatus>(b);

    REQUIRE("memberName1" == fromBinary->characterName()->str());
    REQUIRE(90 == fromBinary->status()->current_life());
    REQUIRE(100 == fromBinary->status()->total_life());
    REQUIRE(190 == fromBinary->status()->current_mp());
    REQUIRE(200 == fromBinary->status()->total_mp());
    REQUIRE(fys::fb::arn::SideOrient_A_N == fromBinary->status()->side());
  }// End section : Binary to Flatbuffer

  SECTION("Test PartyTeamStatus flatbuffer") {
    const fys::fb::arn::CharacterStatus cs2 = fys::fb::arn::CharacterStatus{
        1,
        false,
        1190,
        11100,
        11190,
        11200,
        fys::fb::arn::SideOrient_A_S};
    auto mso2 = fys::fb::arn::CreateMemberStatus(fbb, fbb.CreateString("memberName2"), &cs2);
    fbb.Finish(mso2);
    std::vector<flatbuffers::Offset<fys::fb::arn::MemberStatus>> vec = {mso, mso2};

    auto pts = fys::fb::arn::CreatePartyTeamStatus(
        fbb,
        fbb.CreateString("userName"),
        fbb.CreateVector(vec.data(), 2),
        fbb.CreateVectorOfStrings({"attack1",
                                   "attack2",
                                   "attack3",
                                   "Aattack1",
                                   "Aattack2",
                                   "Aattack3"}));
    fbb.Finish(pts);

    SECTION("Verifier") {
      auto ok = flatbuffers::Verifier(fbb.GetBufferPointer(), fbb.GetSize());
      CHECK(ok.VerifyBuffer<fys::fb::arn::PartyTeamStatus>());
    }

    uint8_t* binary = fbb.GetBufferPointer();

    SECTION("Binary to FlatBuffer") {
      const fys::fb::arn::PartyTeamStatus* fromBinary = flatbuffers::GetRoot<fys::fb::arn::PartyTeamStatus>(binary);
      REQUIRE("userName" == fromBinary->user_name()->str());
      REQUIRE(2 == fromBinary->members()->size());

      auto memberStatusRetrieved1 = fromBinary->members()->Get(0);
      REQUIRE("memberName1" == memberStatusRetrieved1->characterName()->str());
      REQUIRE(90 == memberStatusRetrieved1->status()->current_life());
      REQUIRE(100 == memberStatusRetrieved1->status()->total_life());
      REQUIRE(190 == memberStatusRetrieved1->status()->current_mp());
      REQUIRE(200 == memberStatusRetrieved1->status()->total_mp());
      REQUIRE(fys::fb::arn::SideOrient_A_N == memberStatusRetrieved1->status()->side());

      auto memberStatusRetrieved2 = fromBinary->members()->Get(1);
      REQUIRE("memberName2" == memberStatusRetrieved2->characterName()->str());
      REQUIRE(1190 == memberStatusRetrieved2->status()->current_life());
      REQUIRE(11100 == memberStatusRetrieved2->status()->total_life());
      REQUIRE(11190 == memberStatusRetrieved2->status()->current_mp());
      REQUIRE(11200 == memberStatusRetrieved2->status()->total_mp());
      REQUIRE(fys::fb::arn::SideOrient_A_S == memberStatusRetrieved2->status()->side());

      REQUIRE(6 == fromBinary->attacks()->size());
      REQUIRE("attack1" == fromBinary->attacks()->Get(0)->str());
      REQUIRE("attack2" == fromBinary->attacks()->Get(1)->str());
      REQUIRE("attack3" == fromBinary->attacks()->Get(2)->str());
      REQUIRE("Aattack1" == fromBinary->attacks()->Get(3)->str());
      REQUIRE("Aattack2" == fromBinary->attacks()->Get(4)->str());
      REQUIRE("Aattack3" == fromBinary->attacks()->Get(5)->str());

    }// End section : Binary to Flatbuffer

  }// End section : Test PartyTeamStatus flatbuffer

}// End TestCase : ArenaComFlatBufferTestCase PartyTeamStatus

TEST_CASE("ArenaComFlatBufferTestCase EndBattle", "[service][arena][common][fb]") {
  // test MemberStatus
  flatbuffers::FlatBufferBuilder fbb;

  auto mso = fys::fb::arn::CreateEndBattle(fbb, fys::fb::arn::StatusBattle_WON,
                                           fbb.CreateVector<flatbuffers::Offset<fys::fb::arn::Reward>>({
                                               fys::fb::arn::CreateReward(fbb, fbb.CreateString("reward1"), 42),
                                               fys::fb::arn::CreateReward(fbb, fbb.CreateString("reward2"), 1337),
                                               fys::fb::arn::CreateReward(fbb, fbb.CreateString("reward3"), 1),
                                           }));
  fys::fb::arn::FinishEndBattleBuffer(fbb, mso);

  SECTION("Verifier") {
    auto ok = flatbuffers::Verifier(fbb.GetBufferPointer(), fbb.GetSize());
    CHECK(fys::fb::arn::VerifyEndBattleBuffer(ok));
  }

  uint8_t* b = fbb.GetBufferPointer();

  SECTION("TeamMember Binary to FlatBuffer") {
    const auto* fromBinary = fys::fb::arn::GetEndBattle(b);

    REQUIRE(fys::fb::arn::StatusBattle_WON == fromBinary->status());
    REQUIRE(3 == fromBinary->rewards()->size());
    REQUIRE(42 == fromBinary->rewards()->Get(0)->quantity());
    REQUIRE("reward1" == fromBinary->rewards()->Get(0)->rewardKey()->str());
    REQUIRE(1337 == fromBinary->rewards()->Get(1)->quantity());
    REQUIRE("reward2" == fromBinary->rewards()->Get(1)->rewardKey()->str());
    REQUIRE(1 == fromBinary->rewards()->Get(2)->quantity());
    REQUIRE("reward3" == fromBinary->rewards()->Get(2)->rewardKey()->str());

  }// End section : Binary to Flatbuffer

}// End TestCase : ArenaComFlatBufferTestCase PartyTeamStatus