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

#include <fightingPit/FightingPitAnnouncer.hh>
#include <network/WorkerService.hh>

#include <FSeamMockData.hpp>
#include "TestType.hh"

namespace {
[[nodiscard]] static std::string
getLocalPathStorage()
{
	std::string file_path = __FILE__;
	std::string dir_path = file_path.substr(0, file_path.rfind('\\'));
	if (dir_path.size() == file_path.size())
		dir_path = file_path.substr(0, file_path.rfind('/'));
	return dir_path + "/../../scriptTests/scripts_lnk";
}
}

using namespace fys::arena;

TEST_CASE("WorkerServiceTestCase", "[service][arena]")
{
	fys::arena::WorkerService ws;
	auto cml = CmlBase(getLocalPathStorage());
	EncounterContext ctx;

	SECTION("Failures") {

		REQUIRE(fys::arena::FightingPit::CREATION_ERROR == ws.addFightingPit(nullptr));

	} // End section : Failures

	SECTION("Test addFightingPit") {

		unsigned id1 = ws.addFightingPit(std::make_unique<fys::arena::FightingPit>("1", fys::arena::FightingPit::EASY));
		REQUIRE(1 == id1);
		REQUIRE("1" == fys::arena::FightingPitAnnouncer::getCreatorUserName(ws.getFightingPitInstance(id1)));

		unsigned id2 = ws.addFightingPit(std::make_unique<fys::arena::FightingPit>("2", fys::arena::FightingPit::EASY));
		REQUIRE(2 == id2);
		REQUIRE("2" == fys::arena::FightingPitAnnouncer::getCreatorUserName(ws.getFightingPitInstance(id2)));

		unsigned id3 = ws.addFightingPit(std::make_unique<fys::arena::FightingPit>("3", fys::arena::FightingPit::EASY));
		REQUIRE(3 == id3);
		REQUIRE("3" == fys::arena::FightingPitAnnouncer::getCreatorUserName(ws.getFightingPitInstance(id3)));

		unsigned id4 = ws.addFightingPit(std::make_unique<fys::arena::FightingPit>("4", fys::arena::FightingPit::EASY));
		REQUIRE(4 == id4);
		REQUIRE("4" == fys::arena::FightingPitAnnouncer::getCreatorUserName(ws.getFightingPitInstance(id4)));

	} // End section : Test addFightingPit

	SECTION("Test Player join FightingPit") {
		unsigned id = ws.addFightingPit(std::make_unique<fys::arena::FightingPit>("4", fys::arena::FightingPit::EASY));
		REQUIRE(1 == id);
		REQUIRE("4" == fys::arena::FightingPitAnnouncer::getCreatorUserName(ws.getFightingPitInstance(id)));

		auto& pt = ws.getFightingPitInstance(1u);
		REQUIRE(nullptr != pt);

		ws.playerJoinFightingPit(1u, getPartyTeam("TestUser"), cml);
		REQUIRE(1 == pt->getPartyTeams().getPartyTeams().size());
		REQUIRE(4 == pt->getPartyTeamOfPlayer("TestUser").getTeamMembers().size());

		ws.playerJoinFightingPit(1u, getPartyTeam("TestUser2"), cml);
		REQUIRE(2 == pt->getPartyTeams().getPartyTeams().size());
		REQUIRE(4 == pt->getPartyTeamOfPlayer("TestUser2").getTeamMembers().size());

		pt->setPlayerReadiness("TestUser");
		REQUIRE(pt->isJoinable());
		pt->setPlayerReadiness("TestUser2");
		REQUIRE_FALSE(pt->isJoinable());

		ws.playerJoinFightingPit(41u, getPartyTeam("NOT_GOOD_FP_ID"), cml); // Not existing fighting pit, partyTeam can't join
		REQUIRE(2 == pt->getPartyTeams().getPartyTeams().size());
		REQUIRE_THROWS(pt->getPartyTeamOfPlayer("NOT_GOOD_FP_ID")); // Not joinable isn't present in the pit and this has not been added

		ws.playerJoinFightingPit(1u, getPartyTeam("NOT_JOINABLE"), cml);
		REQUIRE(2 == pt->getPartyTeams().getPartyTeams().size());
		REQUIRE_THROWS(pt->getPartyTeamOfPlayer("NOT_JOINABLE")); // Not joinable isn't present in the pit and this has not been added

	} // End section : Test Player join FightingPit

} // End TestCase : WorkerService test