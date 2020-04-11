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

#include <catch2/catch.hpp>
#include <ArenaServerContext.hh>
#include <fightingPit/FightingPitAnnouncer.hh>
#include "TestType.hh"

#include <fightingPit/contender/FightingContender.hh>
#include <fightingPit/contender/ContenderScripting.hh>

using namespace fys::arena;
using namespace std::chrono_literals;

namespace {

[[nodiscard]] static std::string
getLocalPathStorage()
{
	std::string file_path = __FILE__;
	std::string dir_path = file_path.substr(0, file_path.rfind('\\'));
	if (dir_path.size() == file_path.size())
		dir_path = file_path.substr(0, file_path.rfind('/'));
	return dir_path + "/../scriptTests/scripts_lnk";
}

[[nodiscard]] std::string
getTmpPath()
{
	std::string file_path = __FILE__;
	std::string dir_path = file_path.substr(0, file_path.rfind('\\'));
	if (dir_path.size() == file_path.size())
		dir_path = file_path.substr(0, file_path.rfind('/'));
	return dir_path + "/testCopyTo";
}

}

// Test of the game loop

TEST_CASE("FightingPit test", "[service][arena]")
{
	auto fseamMock = FSeam::getDefault<fys::util::RandomGenerator>();
	ConnectionHandler handler{};
	auto fseamConnectionHandlerMock = FSeam::get(&handler);
	auto cml = CmlBaseCopy(getTmpPath(), getLocalPathStorage());
	DeleteFolderWhenDone raii_delete_folder(getTmpPath());

	EncounterContext ctx;
	ctx._rangeEncounterPerZone["WS00"] = {
			std::make_pair(1, 1), // ez
			std::make_pair(3, 3), // medium
			std::make_pair(1, 1)  // hard
	};
	ctx._contendersPerZone["WS00"] = {
			EncounterContext::EncounterDesc{"testing:TestMonsterSleep.chai", 3, {100, 100, 100}},
	};

	// seed used 42
	std::shared_ptr<std::mt19937> mt = std::make_shared<std::mt19937>(42);
	fseamMock->dupeReturn<FSeam::RandomGenerator::get>(mt);

	SECTION("InGame fighting Ally Team Win") {

		FightingPitAnnouncer fpa(cml);
		fpa.setCreatorUserName("Winner");
		fpa.setCreatorUserToken("WinnerToken");
		fpa.setCreatorTeamParty(getPartyTeam("Winner"));
		fpa.setDifficulty(FightingPit::EASY);
		fpa.setEncounterType(FightingPitAnnouncer::EncounterType::RANDOM);

		fpa.addActionToOneMember(0u, "arena:actions:damage:slash.chai", 1u); // slash level 1 do 33 damage
		fpa.addActionToOneMember(1u, "arena:actions:damage:slash.chai", 2u); // slash level 2 do 36 damage
		fpa.addActionToOneMember(2u, "arena:actions:damage:slash.chai", 3u); // slash level 3 do 39 damage
		fpa.addActionToOneMember(3u, "arena:actions:damage:slash.chai", 4u); // slash level 4 do 42 damage

		auto fp = fpa.buildFightingPit(ctx, "WS00");

		const auto& contender = FightingPitAnnouncer::getPitContenders(fp).getContenders().at(0);
		REQUIRE(1 == FightingPitAnnouncer::getPitContenders(fp).getNumberContender());
		REQUIRE("TestMonsterSleep" == contender->getContenderScripting()->getContenderName());
		REQUIRE(100 == contender->getStatus().life.current);
		REQUIRE(100 == contender->getStatus().life.total);

		const auto& c = FightingPitAnnouncer::getPitContenders(fp).getContenderOnSide(HexagonSide::Orientation::B_S);
		const auto& p = FightingPitAnnouncer::getPartyTeams(fp).getMembersBySide(HexagonSide::Orientation::B_S);

		SECTION("Battle Order") {

			auto& side = FightingPitAnnouncer::getSideBattleForSide(fp, HexagonSide::Orientation::B_S);
			auto now = std::chrono::system_clock::now();

			REQUIRE_FALSE(side.empty());
			REQUIRE(1 == FightingPitAnnouncer::getPitContenders(fp).getNumberContender());
			REQUIRE(1 == FightingPitAnnouncer::getPitContenders(fp).contenderOnSide(HexagonSide::Orientation::B_S));
			REQUIRE(4 == FightingPitAnnouncer::getPartyTeams(fp).getNumberAlly());

			REQUIRE(8 == c.at(0)->getStatus().initialSpeed);
			REQUIRE(3 == p.at(0)->getStatus().initialSpeed);
			REQUIRE(5 == p.at(1)->getStatus().initialSpeed);
			REQUIRE(10 == p.at(2)->getStatus().initialSpeed);
			REQUIRE(20 == p.at(3)->getStatus().initialSpeed);

			fys::arena::data::PriorityElem pe = side.getCurrentParticipantTurn(now, 0ms);
			REQUIRE_FALSE(pe.isContender);
			REQUIRE(20 == pe.speed);
			REQUIRE(4 == pe.id);

			pe = side.getCurrentParticipantTurn(now, 0ms);
			REQUIRE_FALSE(pe.isContender);
			REQUIRE(10 == pe.speed);
			REQUIRE(3 == pe.id);

			pe = side.getCurrentParticipantTurn(now, 0ms);
			REQUIRE(pe.isContender);
			REQUIRE(8 == pe.speed);
			REQUIRE(0 == pe.id);

			pe = side.getCurrentParticipantTurn(now, 0ms);
			REQUIRE_FALSE(pe.isContender);
			REQUIRE(5 == pe.speed);
			REQUIRE(2 == pe.id);

			pe = side.getCurrentParticipantTurn(now, 0ms);
			REQUIRE_FALSE(pe.isContender);
			REQUIRE(3 == pe.speed);
			REQUIRE(1 == pe.id);

		} // End section : Battle Order

		REQUIRE(FightingPitAnnouncer::isOnHold(fp));
		fp->setPlayerReadiness("Winner");
		REQUIRE(FightingPitAnnouncer::isOnGoing(fp));

		SECTION("Battle Turn 1 test") {

			p.at(0)->addPendingAction("slash", ContenderTargetId{0});
			p.at(1)->addPendingAction("slash", ContenderTargetId{0});
			p.at(2)->addPendingAction("slash", ContenderTargetId{0});
			p.at(3)->addPendingAction("slash", ContenderTargetId{0});
			auto now = std::chrono::system_clock::now();
			fp->continueBattle(now);

			now += fys::arena::FightingPit::EASY_INTERVAL + 1ms;
			fp->continueBattle(now);

			now += fys::arena::FightingPit::EASY_INTERVAL + 1ms;
//			fp->continueBattle(now);
//
			now += fys::arena::FightingPit::EASY_INTERVAL + 1ms;
//			fp->continueBattle(now);
//
			now += fys::arena::FightingPit::EASY_INTERVAL + 1ms;
//			fp->continueBattle(now);

		} // End section : Battle Turn 1 test

	} // End section : InGame fighting

	// cleanup
	FSeam::MockVerifier::cleanUp();

} // End TestCase : FightingPit test

#pragma clang diagnostic pop