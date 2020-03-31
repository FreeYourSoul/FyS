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

#include <catch2/catch.hpp>
#include <flatbuffers/flatbuffers.h>
#include <ArenaServerAuth_generated.h>
#include <ArenaServerValidateAuth_generated.h>
#include <PartyTeamStatus_generated.h>
#include <FightingPitEncounter_generated.h>
#include <ArenaFightAction_generated.h>

TEST_CASE("FlatBuffer ArenaCom FightingPitEncounter", "[service][arena][common][fb]")
{
	flatbuffers::FlatBufferBuilder fbb;
	auto fp = fys::fb::CreateFightingPitEncounter(
			fbb,
			fbb.CreateString("name"),
			fbb.CreateString("token_authentication"),
			1337,
			fbb.CreateString("WS001"),
			fys::fb::Level::Level_EASY,
			4242,
			false,
			0.0,
			0.0);
	fys::fb::FinishFightingPitEncounterBuffer(fbb, fp);

	SECTION("Verifier") {
		auto ok = flatbuffers::Verifier(fbb.GetBufferPointer(), fbb.GetSize());
		CHECK(fys::fb::VerifyFightingPitEncounterBuffer(ok));
	}
	uint8_t* binary = fbb.GetBufferPointer();

	SECTION("Binary to FlatBuffer") {
		const fys::fb::FightingPitEncounter* fromBinary = fys::fb::GetFightingPitEncounter(binary);
		REQUIRE("name" == fromBinary->user_name()->str());
		REQUIRE("token_authentication" == fromBinary->token_auth()->str());
		REQUIRE(4242 == fromBinary->id_encounter());
		REQUIRE_FALSE(fromBinary->is_ambush());
		REQUIRE(1337 == fromBinary->fighting_pit_id());

	} // End section : Binary to Flatbuffer

} // // End Test Case : FlatBuffer ArenaCom FightingPitEncounter

TEST_CASE("FlatBuffer ArenaCom ArenaServerAuth", "[service][arena][common][fb]")
{

	flatbuffers::FlatBufferBuilder fbb;
	auto asa = fys::fb::CreateArenaServerAuth(
			fbb,
			fbb.CreateString("name"),
			fbb.CreateString("token_authentication"),
			fbb.CreateString("localhost"),
			fbb.CreateString("tcp://localhost:4242"),
			42,
			fbb.CreateString("WS001"));
	fys::fb::FinishArenaServerAuthBuffer(fbb, asa);

	SECTION("Verifier") {
		auto ok = flatbuffers::Verifier(fbb.GetBufferPointer(), fbb.GetSize());
		CHECK(fys::fb::VerifyArenaServerAuthBuffer(ok));
	}
	uint8_t* binary = fbb.GetBufferPointer();

	SECTION("Binary to FlatBuffer") {
		const fys::fb::ArenaServerAuth* fromBinary = fys::fb::GetArenaServerAuth(binary);
		REQUIRE("name" == fromBinary->user_name()->str());
		REQUIRE("token_authentication" == fromBinary->token_auth()->str());
		REQUIRE("localhost" == fromBinary->ip()->str());
		REQUIRE("tcp://localhost:4242" == fromBinary->connection_str()->str());
		REQUIRE("WS001" == fromBinary->arena_server_code()->str());
		REQUIRE(42 == fromBinary->id_fightingpit());

	} // End section : Binary to Flatbuffer

} // End Test Case : FlatBuffer ArenaCom ArenaServerAuth

TEST_CASE("FlatBuffer ArenaCom ArenaServerValidateAuth", "[service][arena][common][fb]")
{

	flatbuffers::FlatBufferBuilder fbb;
	auto asva = fys::fb::CreateArenaServerValidateAuth(
			fbb,
			fbb.CreateString("name"),
			fbb.CreateString("token_authentication"),
			42);
	fys::fb::FinishArenaServerValidateAuthBuffer(fbb, asva);

	SECTION("Verifier") {
		auto ok = flatbuffers::Verifier(fbb.GetBufferPointer(), fbb.GetSize());
		CHECK(fys::fb::VerifyArenaServerValidateAuthBuffer(ok));
	}

	SECTION("Binary to FlatBuffer") {
		uint8_t* binary = fbb.GetBufferPointer();
		const fys::fb::ArenaServerValidateAuth* fromBinary = fys::fb::GetArenaServerValidateAuth(binary);
		REQUIRE("name" == fromBinary->user_name()->str());
		REQUIRE("token_authentication" == fromBinary->token_auth()->str());
		REQUIRE(42 == fromBinary->fighting_pit_id());

	} // End section : Binary to Flatbuffer

} // End Test Case : FlatBuffer ArenaCom ArenaServerValidateAuth

TEST_CASE("FlatBuffer ArenaCom ArenaFightAction", "[service][arena][common][fb]")
{

	flatbuffers::FlatBufferBuilder fbb;
	auto afa = fys::fb::CreateArenaFightAction(
			fbb,
			1337,
			fbb.CreateString("idAction"),
			fys::fb::Targeting_ENNEMIES,
			fbb.CreateVector(std::vector<uint> {1, 4, 5, 6}));
	fys::fb::FinishArenaFightActionBuffer(fbb, afa);

	SECTION("Verifier") {
		auto ok = flatbuffers::Verifier(fbb.GetBufferPointer(), fbb.GetSize());
		CHECK(fys::fb::VerifyArenaFightActionBuffer(ok));
	}

	SECTION("Binary to FlatBuffer") {
		uint8_t* binary = fbb.GetBufferPointer();
		const fys::fb::ArenaFightAction* fromBinary = fys::fb::GetArenaFightAction(binary);
		REQUIRE("idAction" == fromBinary->actionId()->str());
		REQUIRE(1337 == fromBinary->memberId());
		REQUIRE(fys::fb::Targeting_ENNEMIES == fromBinary->targetType());
		REQUIRE(1 == fromBinary->targetId()->Get(0));
		REQUIRE(4 == fromBinary->targetId()->Get(1));
		REQUIRE(5 == fromBinary->targetId()->Get(2));
		REQUIRE(6 == fromBinary->targetId()->Get(3));

	} // End section : Binary to Flatbuffer

} // End Test Case : FlatBuffer ArenaCom ArenaFightAction

TEST_CASE("FlatBuffer ArenaCom PartyTeamStatus", "[service][arena][common][fb]")
{
	// test MemberStatus
	flatbuffers::FlatBufferBuilder fbb;

	auto mso = fys::fb::CreateMemberStatus(fbb, fbb.CreateString("memberName1"),
			90,
			100,
			190,
			200,
			fys::fb::SideOrient_A_N,
			fbb.CreateVectorOfStrings({"attack1", "attack2", "attack3"}));
	fbb.Finish(mso);
	uint8_t* b = fbb.GetBufferPointer();

	SECTION("TeamMember Binary to FlatBuffer") {
		const auto* fromBinary = flatbuffers::GetRoot<fys::fb::MemberStatus>(b);

		REQUIRE("memberName1" == fromBinary->memberName()->str());
		REQUIRE(90 == fromBinary->current_life());
		REQUIRE(100 == fromBinary->total_life());
		REQUIRE(190 == fromBinary->current_mp());
		REQUIRE(200 == fromBinary->total_mp());
		REQUIRE(fys::fb::SideOrient_A_N == fromBinary->side());
		REQUIRE(3 == fromBinary->attacks()->size());
		REQUIRE("attack1" == fromBinary->attacks()->Get(0)->str());
		REQUIRE("attack2" == fromBinary->attacks()->Get(1)->str());
		REQUIRE("attack3" == fromBinary->attacks()->Get(2)->str());
	} // End section : Binary to Flatbuffer


	SECTION("Test PartyTeamStatus flatbuffer") {
		auto mso2 = fys::fb::CreateMemberStatus(fbb, fbb.CreateString("memberName2"),
				1190,
				11100,
				11190,
				11200,
				fys::fb::SideOrient_A_S,
				fbb.CreateVectorOfStrings({"Aattack1", "Aattack2", "Aattack3"}));
		fbb.Finish(mso2);
		std::vector<flatbuffers::Offset<fys::fb::MemberStatus>> vec = {mso, mso2};

		auto pts = fys::fb::CreatePartyTeamStatus(
				fbb,
				fbb.CreateString("userName"),
				fbb.CreateVector(vec.data(), 2));
		fys::fb::FinishPartyTeamStatusBuffer(fbb, pts);

		SECTION("Verifier") {
			auto ok = flatbuffers::Verifier(fbb.GetBufferPointer(), fbb.GetSize());
			CHECK(fys::fb::VerifyPartyTeamStatusBuffer(ok));
		}
		uint8_t* binary = fbb.GetBufferPointer();

		SECTION("Binary to FlatBuffer") {
			const fys::fb::PartyTeamStatus* fromBinary = fys::fb::GetPartyTeamStatus(binary);
			REQUIRE("userName" == fromBinary->user_name()->str());
			REQUIRE(2 == fromBinary->members()->size());

			auto memberStatusRetrieved1 = fromBinary->members()->Get(0);
			REQUIRE("memberName1" == memberStatusRetrieved1->memberName()->str());
			REQUIRE(90 == memberStatusRetrieved1->current_life());
			REQUIRE(100 == memberStatusRetrieved1->total_life());
			REQUIRE(190 == memberStatusRetrieved1->current_mp());
			REQUIRE(200 == memberStatusRetrieved1->total_mp());
			REQUIRE(fys::fb::SideOrient_A_N == memberStatusRetrieved1->side());
			REQUIRE(3 == memberStatusRetrieved1->attacks()->size());
			REQUIRE("attack1" == memberStatusRetrieved1->attacks()->Get(0)->str());
			REQUIRE("attack2" == memberStatusRetrieved1->attacks()->Get(1)->str());
			REQUIRE("attack3" == memberStatusRetrieved1->attacks()->Get(2)->str());

			auto memberStatusRetrieved2 = fromBinary->members()->Get(1);
			REQUIRE("memberName2" == memberStatusRetrieved2->memberName()->str());
			REQUIRE(1190 == memberStatusRetrieved2->current_life());
			REQUIRE(11100 == memberStatusRetrieved2->total_life());
			REQUIRE(11190 == memberStatusRetrieved2->current_mp());
			REQUIRE(11200 == memberStatusRetrieved2->total_mp());
			REQUIRE(fys::fb::SideOrient_A_S == memberStatusRetrieved2->side());
			REQUIRE(3 == memberStatusRetrieved2->attacks()->size());
			REQUIRE("Aattack1" == memberStatusRetrieved2->attacks()->Get(0)->str());
			REQUIRE("Aattack2" == memberStatusRetrieved2->attacks()->Get(1)->str());
			REQUIRE("Aattack3" == memberStatusRetrieved2->attacks()->Get(2)->str());

		} // End section : Binary to Flatbuffer
	} // End section : Test PartyTeamStatus flatbuffer

} // End TestCase : FlatBuffer ArenaCom PartyTeamStatus