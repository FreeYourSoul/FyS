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

	SECTION("InGame fighting Ally Team Win (One Contender)") {

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

		auto& chai = fp->getChaiPtr();

		const auto& contender = FightingPitAnnouncer::getPitContenders(fp).getContenders().at(0);
		REQUIRE(1 == FightingPitAnnouncer::getPitContenders(fp).getNumberContender());
		REQUIRE("TestMonsterSleep" == contender->getContenderScripting()->getContenderName());
		REQUIRE(100 == contender->getStatus().life.current);
		REQUIRE(100 == contender->getStatus().life.total);

		const auto& c = FightingPitAnnouncer::getPitContenders(fp).getContenderOnSide(HexagonSide::Orientation::B_S);
		const auto& p = FightingPitAnnouncer::getPartyTeams(fp).getMembersBySide(HexagonSide::Orientation::B_S);

		REQUIRE(fp->isJoinable());
		fp->setPlayerReadiness("Winner");
		REQUIRE_FALSE(fp->isJoinable());
		REQUIRE(FightingPitAnnouncer::isOnGoing(fp));

		SECTION("Battle Turn 1 test") {

			p.at(0)->addPendingAction("slash", ContenderTargetId{0});
			p.at(1)->addPendingAction("slash", ContenderTargetId{0});
			p.at(2)->addPendingAction("slash", ContenderTargetId{0});
			p.at(3)->addPendingAction("slash", ContenderTargetId{0});

			auto now = std::chrono::system_clock::now();

			try {
				// Player id 4 (at index 3) is going to slash the only opponent it has inflicting 42 damage
				// The ennemy total life is 100, goes to 58
				fp->continueBattle(now);
				REQUIRE(58  == FightingPitAnnouncer::getPitContenders(fp).getContenders().at(0)->getStatus().life.current);
				REQUIRE(100 == FightingPitAnnouncer::getPitContenders(fp).getContenders().at(0)->getStatus().life.total);

				// Player id 3 (at index 2) is going to slash the only opponent it has inflicting 39 damage
				// The ennemy total life is 100, current from 58 goes to 19
				now += fys::arena::interval::EASY + 1ms;
				fp->continueBattle(now);
				REQUIRE(19  == FightingPitAnnouncer::getPitContenders(fp).getContenders().at(0)->getStatus().life.current);
				REQUIRE(100 == FightingPitAnnouncer::getPitContenders(fp).getContenders().at(0)->getStatus().life.total);

				// Contender id 0 is going to Sleep and so restore 40 hp
				// The ennemy total life is 100, current from 19 goes to 59
				now += fys::arena::interval::EASY + 1ms;
				fp->continueBattle(now);
				REQUIRE(59  == FightingPitAnnouncer::getPitContenders(fp).getContenders().at(0)->getStatus().life.current);
				REQUIRE(100 == FightingPitAnnouncer::getPitContenders(fp).getContenders().at(0)->getStatus().life.total);

				// Player id 2 (at index 1) is going to slash the only opponent it has inflicting 36 damage
				// The ennemy total life is 100, current from 59 goes to 23
				now += fys::arena::interval::EASY + 1ms;
				fp->continueBattle(now);
				REQUIRE(23  == FightingPitAnnouncer::getPitContenders(fp).getContenders().at(0)->getStatus().life.current);
				REQUIRE(100 == FightingPitAnnouncer::getPitContenders(fp).getContenders().at(0)->getStatus().life.total);

				// Player id 2 (at index 1) is going to slash the only opponent it has inflicting 33 damage
				// The ennemy total life is 100, current from 23 goes to 0 (overkill)
				now += fys::arena::interval::EASY + 1ms;
				REQUIRE(FightingPitAnnouncer::isOnGoing(fp));
				fp->continueBattle(now);
				REQUIRE(0  == FightingPitAnnouncer::getPitContenders(fp).getContenders().at(0)->getStatus().life.current);
				REQUIRE(100 == FightingPitAnnouncer::getPitContenders(fp).getContenders().at(0)->getStatus().life.total);

				// Battle ended checks
				REQUIRE_FALSE(FightingPitAnnouncer::isOnGoing(fp));
				REQUIRE(FightingPitAnnouncer::isAllyWin(fp));

				// Battle is ended and so nothing happens
				// todo check the call to connection manager with the reply latter on
				now += fys::arena::interval::EASY + 1ms;
				fp->continueBattle(now);

				// Battle is now ready to be cleaned up
				REQUIRE(fp->isBattleOver());
			}
			catch (const std::exception& e) {
				SPDLOG_ERROR("Exception during Battle turn 1 test {} ", e.what());
				FAIL("Should not arrive here");
			}

		} // End section : Battle Turn 1 test

	} // End section : InGame fighting

	SECTION("InGame fighting Contender Team Win (One Contender)") {

		ctx._contendersPerZone["WS00"] = {
				EncounterContext::EncounterDesc{"testing:TestMonsterAttack.chai", 3, {100, 100, 100}},
		};

		FightingPitAnnouncer fpa(cml);
		fpa.setCreatorUserName("Loser");
		fpa.setCreatorUserToken("LoserToken");
		auto partyTeam = getPartyTeam("Loser");
		partyTeam->accessTeamMembers().at(0)->accessStatus().magicPoint.current = 0;
		partyTeam->accessTeamMembers().at(1)->accessStatus().magicPoint.current = 0;
		partyTeam->accessTeamMembers().at(2)->accessStatus().magicPoint.current = 0;
		partyTeam->accessTeamMembers().at(2)->accessStatus().life.current = 99; // set to the lowest life in order to ensure as target
		partyTeam->accessTeamMembers().at(3)->accessStatus().magicPoint.current = 0;
		fpa.setCreatorTeamParty(std::move(partyTeam));
		fpa.setDifficulty(FightingPit::EASY);
		fpa.setEncounterType(FightingPitAnnouncer::EncounterType::RANDOM);

		fpa.addActionToOneMember(0u, "arena:actions:misc:meditate.chai", 1u); // meditate level 1 restore 10 mp
		fpa.addActionToOneMember(1u, "arena:actions:misc:meditate.chai", 2u); // meditate level 2 restore 20 mp
		fpa.addActionToOneMember(2u, "arena:actions:misc:meditate.chai", 3u); // meditate level 3 restore 30 mp
		fpa.addActionToOneMember(3u, "arena:actions:misc:meditate.chai", 4u); // meditate level 4 restore 40 mp

		auto fp = fpa.buildFightingPit(ctx, "WS00");

		auto& chai = fp->getChaiPtr();

		REQUIRE(1 == FightingPitAnnouncer::getPitContenders(fp).getNumberContender());
		const auto& contender = FightingPitAnnouncer::getPitContenders(fp).getContenders().at(0);
		const auto& c = FightingPitAnnouncer::getPitContenders(fp).getContenderOnSide(HexagonSide::Orientation::B_S);
		const auto& p = FightingPitAnnouncer::getPartyTeams(fp).getMembersBySide(HexagonSide::Orientation::B_S);

		SECTION("Initial setup") {
			REQUIRE("TestMonsterAttack" == contender->getContenderScripting()->getContenderName());
			REQUIRE(4 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers().size());
			REQUIRE(100 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[0]->getStatus().life.current);
			REQUIRE(100 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[0]->getStatus().life.total);
			REQUIRE(0 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[0]->getStatus().magicPoint.current);
			REQUIRE(200 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[1]->getStatus().life.current);
			REQUIRE(200 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[1]->getStatus().life.total);
			REQUIRE(0 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[0]->getStatus().magicPoint.current);
			REQUIRE(99 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[2]->getStatus().life.current); // <<< LOWEST LIFE TARGET
			REQUIRE(550 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[2]->getStatus().life.total);
			REQUIRE(0 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[0]->getStatus().magicPoint.current);
			REQUIRE(140 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[3]->getStatus().life.current);
			REQUIRE(140 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[3]->getStatus().life.total);
			REQUIRE(0 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[0]->getStatus().magicPoint.current);

			REQUIRE(1 == FightingPitAnnouncer::getPitContenders(fp).getNumberContender());
			REQUIRE(1 == FightingPitAnnouncer::getPitContenders(fp).contenderOnSide(HexagonSide::Orientation::B_S));
			REQUIRE(4 == FightingPitAnnouncer::getPartyTeams(fp).getNumberAlly());

			REQUIRE(8 == c.at(0)->getStatus().initialSpeed);
			REQUIRE(3 == p.at(0)->getStatus().initialSpeed);
			REQUIRE(5 == p.at(1)->getStatus().initialSpeed);
			REQUIRE(10 == p.at(2)->getStatus().initialSpeed);
			REQUIRE(20 == p.at(3)->getStatus().initialSpeed);
		} // End section : Initial setup

		REQUIRE(fp->isJoinable());
		fp->setPlayerReadiness("Loser");
		REQUIRE_FALSE(fp->isJoinable());
		REQUIRE(FightingPitAnnouncer::isOnGoing(fp));

		// other turns are tested in the test  Test for FightingPit testcase in file PriorityOrderListTestCase.cpp

		SECTION("Battle Turn 1 test") {

			p.at(0)->addPendingAction("meditate", std::nullopt);
			p.at(1)->addPendingAction("meditate", std::nullopt);
			p.at(2)->addPendingAction("meditate", std::nullopt);
			p.at(3)->addPendingAction("meditate", std::nullopt);

			auto now = std::chrono::system_clock::now();

			try {
				// Player id 4 (at index 3) is going to meditate, restore 40 mp, from 0 mp to 10 (max mp 10)
				fp->continueBattle(now);
				REQUIRE(10 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[3]->getStatus().magicPoint.current);
				REQUIRE(0 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[2]->getStatus().magicPoint.current);
				REQUIRE(0 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[1]->getStatus().magicPoint.current);
				REQUIRE(0 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[0]->getStatus().magicPoint.current);

				// Player id 3 (at index 2) is going to meditate, restore 30 mp, from 0 mp to 10 (max mp 10)
				now += fys::arena::interval::EASY + 1ms;
				fp->continueBattle(now);
				REQUIRE(10 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[2]->getStatus().magicPoint.current);
				REQUIRE(0 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[1]->getStatus().magicPoint.current);
				REQUIRE(0 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[0]->getStatus().magicPoint.current);

				// Contender turn : Target lowest life opponent:
				// ally id 1  =  100
				// ally id 2  =  200
				// ally id 3  =  99  << Target -> damage done by attack 90 damage << 9 life left
				// ally id 4  =  140
				now += fys::arena::interval::EASY + 1ms;
				fp->continueBattle(now);
				REQUIRE(9   == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[2]->getStatus().life.current); // damage inflicted
				REQUIRE(550 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[2]->getStatus().life.total);

				// Stay same
				REQUIRE(100 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[0]->getStatus().life.current);
				REQUIRE(100 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[0]->getStatus().life.total);
				REQUIRE(200 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[1]->getStatus().life.current);
				REQUIRE(200 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[1]->getStatus().life.total);
				REQUIRE(140 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[3]->getStatus().life.current);
				REQUIRE(140 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[3]->getStatus().life.total);

				// Player id 2 (at index 1) is going to meditate, restore 20 mp, from 0 mp to 0 (max mp 0)
				now += fys::arena::interval::EASY + 1ms;
				fp->continueBattle(now);
				REQUIRE(0 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[1]->getStatus().magicPoint.current);
				REQUIRE(0 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[0]->getStatus().magicPoint.current);

				// Player id 1 (at index 0) is going to meditate, restore 10 mp, from 0 mp to 10 (max mp 20)
				now += fys::arena::interval::EASY + 1ms;
				REQUIRE(FightingPitAnnouncer::isOnGoing(fp));
				fp->continueBattle(now);
				REQUIRE(10 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[0]->getStatus().magicPoint.current);

				SECTION("Battle Turn 2 test") { // a4, a4, a3, c0, a2, c0, a1
					p.at(1)->addPendingAction("meditate", std::nullopt);
					p.at(2)->addPendingAction("meditate", std::nullopt);
					p.at(3)->addPendingAction("meditate", std::nullopt);

					// Player id 4 (at index 3) is going to meditate, restore 20 mp, from 10 mp to 10 (max mp 10)
					now += fys::arena::interval::EASY + 1ms;
					fp->continueBattle(now);
					REQUIRE(10 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[3]->getStatus().magicPoint.current);
					REQUIRE(10 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[2]->getStatus().magicPoint.current);
					REQUIRE(0 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[1]->getStatus().magicPoint.current);
					REQUIRE(10 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[0]->getStatus().magicPoint.current);

					// Player id 4 (at index 3) is going to meditate, restore 20 mp, from 10 mp to 10 (max mp 10)
					now += fys::arena::interval::EASY + 1ms;
					fp->continueBattle(now);
					REQUIRE(10 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[3]->getStatus().magicPoint.current);
					REQUIRE(10 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[2]->getStatus().magicPoint.current);
					REQUIRE(0 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[1]->getStatus().magicPoint.current);
					REQUIRE(10 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[0]->getStatus().magicPoint.current);

					// Player id 3 (at index 2) is going to meditate, restore 30 mp, from 10 mp to 10 (max mp 10)
					now += fys::arena::interval::EASY + 1ms;
					fp->continueBattle(now);
					REQUIRE(10 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[3]->getStatus().magicPoint.current);
					REQUIRE(10 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[2]->getStatus().magicPoint.current);
					REQUIRE(0 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[1]->getStatus().magicPoint.current);
					REQUIRE(10 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[0]->getStatus().magicPoint.current);

					// Contender turn : Target lowest life opponent:
					// ally id 1  =  100
					// ally id 2  =  200
					// ally id 3  =  9   << Target -> damage done by attack 90 damage << 0 life left (DEAD)
					// ally id 4  =  140
					now += fys::arena::interval::EASY + 1ms;
					fp->continueBattle(now);
					REQUIRE(0 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[2]->getStatus().life.current);
					REQUIRE(fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[2]->getStatus().life.isDead());
					// Stay same
					REQUIRE(100 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[0]->getStatus().life.current); // damage inflicted
					REQUIRE(100 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[0]->getStatus().life.total);
					REQUIRE(200 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[1]->getStatus().life.current);
					REQUIRE(200 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[1]->getStatus().life.total);
					REQUIRE(550 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[2]->getStatus().life.total);
					REQUIRE(140 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[3]->getStatus().life.current);
					REQUIRE(140 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[3]->getStatus().life.total);
					// Player died and is not in game anymore, he won't play next turn

					// Player id 2 (at index 1) is going to meditate, restore 10 mp, from 0 mp to 0 (max mp 0)
					now += fys::arena::interval::EASY + 1ms;
					fp->continueBattle(now);
					REQUIRE(10 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[3]->getStatus().magicPoint.current);
					REQUIRE(10 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[2]->getStatus().magicPoint.current);
					REQUIRE(0 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[1]->getStatus().magicPoint.current);
					REQUIRE(10 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[0]->getStatus().magicPoint.current);

					// Contender turn : Target lowest life opponent:
					// ally id 1  =  100 << target from 100 to 10
					// ally id 2  =  200
					// ally id 3  =  0 (DEAD)
					// ally id 4  =  140
					now += fys::arena::interval::EASY + 1ms;
					fp->continueBattle(now);
					REQUIRE(10 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[0]->getStatus().life.current); // damage inflicted
					REQUIRE(100 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[0]->getStatus().life.total);
					// Stay same
					REQUIRE(0 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[2]->getStatus().life.current);
					REQUIRE(fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[2]->getStatus().life.isDead());
					REQUIRE(200 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[1]->getStatus().life.current);
					REQUIRE(200 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[1]->getStatus().life.total);
					REQUIRE(550 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[2]->getStatus().life.total);
					REQUIRE(140  == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[3]->getStatus().life.current);
					REQUIRE(140 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[3]->getStatus().life.total);

					// Player id 1 (at index 0) is going to meditate, restore 10 mp, from 10 mp to 10 (max mp 10)
					now += fys::arena::interval::EASY + 1ms;
					fp->continueBattle(now);
					REQUIRE(10 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[3]->getStatus().magicPoint.current);
					REQUIRE(10 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[2]->getStatus().magicPoint.current);
					REQUIRE(0 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[1]->getStatus().magicPoint.current);
					REQUIRE(10 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[0]->getStatus().magicPoint.current);

					SECTION("Battle turn 3 test") {
						p.at(0)->addPendingAction("meditate", std::nullopt);
						p.at(2)->addPendingAction("meditate", std::nullopt);
						p.at(3)->addPendingAction("meditate", std::nullopt);

						// Player id 4 (at index 3)
						now += fys::arena::interval::EASY + 1ms;
						fp->continueBattle(now);

						// Contender
						now += fys::arena::interval::EASY + 1ms;
						fp->continueBattle(now);
						REQUIRE(0 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[0]->getStatus().life.current); // damage inflicted
						REQUIRE(fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[0]->getStatus().life.isDead());
						REQUIRE(100 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[0]->getStatus().life.total);
						// Stay same
						REQUIRE(0 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[2]->getStatus().life.current);
						REQUIRE(fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[2]->getStatus().life.isDead());
						REQUIRE(200 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[1]->getStatus().life.current);
						REQUIRE(200 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[1]->getStatus().life.total);
						REQUIRE(550 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[2]->getStatus().life.total);
						REQUIRE(140  == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[3]->getStatus().life.current);
						REQUIRE(140 == fp->getPartyTeamOfPlayer("Loser").getTeamMembers()[3]->getStatus().life.total);

						// Player id 2 (at index 1)
						now += fys::arena::interval::EASY + 1ms;
						fp->continueBattle(now);

						// Player id 4 (at index 1) is going to meditate, restore 10 mp, from 0 mp to 0 (max mp 0)
						now += fys::arena::interval::EASY + 1ms;
						fp->continueBattle(now);

						// Player id 1 (at index 0) is going to meditate, restore 10 mp, from 20 mp to 20 (max mp 20)
						now += fys::arena::interval::EASY + 1ms;
						fp->continueBattle(now);

					} // End section : Battle turn 3 test

				} // End section : Battle Turn 2 test


//				// Battle ended checks
//				REQUIRE_FALSE(FightingPitAnnouncer::isOnGoing(fp));
//				REQUIRE(FightingPitAnnouncer::isContenderWin(fp));
//
//				// Battle is ended and so nothing happens
//				// todo check the call to connection manager with the reply latter on
//				now += fys::arena::interval::EASY + 1ms;
//				fp->continueBattle(now);
//
//				// Battle is now ready to be cleaned up
//				REQUIRE(fp->isBattleOver());
			}
			catch (const std::exception& e) {
				SPDLOG_ERROR("Exception during Battle turn 1 test {} ", e.what());
				FAIL("Should not arrive here");
			}

		} // End section : Battle Turn 1 test

	} // End section : InGame fighting

	// cleanup
	FSeam::MockVerifier::cleanUp();

} // End TestCase : FightingPit test


#pragma clang diagnostic pop