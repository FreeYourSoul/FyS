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
#include <FSeamMockData.hpp>
#include <ArenaServerContext.hh>
#include <fightingPit/data/CommonTypes.hh>
#include <fightingPit/FightingPitAnnouncer.hh>
#include <fightingPit/team/TeamMember.hh>
#include <fightingPit/contender/FightingContender.hh>
#include <fightingPit/contender/ContenderScripting.hh>
#include "TestType.hh"

using namespace fys::arena;

TEST_CASE("FightingPitAnnouncer test", "[service][arena]")
{
	auto fseamMock = FSeam::getDefault<fys::util::RandomGenerator>();
	ConnectionHandler handler{};
	auto fseamConnectionHandlerMock = FSeam::get(&handler);
	auto cml = CmlBase(getLocalPathStorage());
	EncounterContext ctx;
	ctx._rangeEncounterPerZone["WS00"] = {
			std::make_pair(1, 4), // ez
			std::make_pair(2, 4), // medium
			std::make_pair(3, 5)  // hard
	};
	ctx._contendersPerZone["WS00"] = {
			EncounterContext::EncounterDesc{
					"arena:contenders:Sampy.chai", 3,
					{60, 60, 60}
			},
			EncounterContext::EncounterDesc{
					"arena:contenders:Slime.chai", 3,
					{40, 40, 40}
			}
	};

	SECTION("test seed 42") {
		std::shared_ptr<std::mt19937> mt = std::make_shared<std::mt19937>(42);
		fseamMock->dupeReturn<FSeam::RandomGenerator::get>(mt);

		SECTION("test seed ez") {
			REQUIRE(2 == fys::util::RandomGenerator::generateInRange(1, 4));
			// encounter 1
			REQUIRE(80 == fys::util::RandomGenerator::generateInRange(0, 100));
			REQUIRE(10 == fys::util::RandomGenerator::generateInRange(1, 10));
			// encounter 2
			REQUIRE(18 == fys::util::RandomGenerator::generateInRange(0, 100));
			REQUIRE(8 == fys::util::RandomGenerator::generateInRange(1, 10));

		} // End section : test seed ez
		SECTION("test seed medium") {
			REQUIRE(3 == fys::util::RandomGenerator::generateInRange(2, 4));
			// encounter 1
			REQUIRE(80 == fys::util::RandomGenerator::generateInRange(0, 100));
			REQUIRE(10 == fys::util::RandomGenerator::generateInRange(1, 10));
			// encounter 2
			REQUIRE(18 == fys::util::RandomGenerator::generateInRange(0, 100));
			REQUIRE(8 == fys::util::RandomGenerator::generateInRange(1, 10));
			// encounter 3
			REQUIRE(78 == fys::util::RandomGenerator::generateInRange(0, 100));
			REQUIRE(6 == fys::util::RandomGenerator::generateInRange(1, 10));
		} // End section : test seed medium

		SECTION("test seed hard") {
			REQUIRE(4 == fys::util::RandomGenerator::generateInRange(3, 5));
			// encounter 1
			REQUIRE(80 == fys::util::RandomGenerator::generateInRange(0, 100));
			REQUIRE(10 == fys::util::RandomGenerator::generateInRange(1, 10));
			// encounter 2
			REQUIRE(18 == fys::util::RandomGenerator::generateInRange(0, 100));
			REQUIRE(8 == fys::util::RandomGenerator::generateInRange(1, 10));
			// encounter 3
			REQUIRE(78 == fys::util::RandomGenerator::generateInRange(0, 100));
			REQUIRE(6 == fys::util::RandomGenerator::generateInRange(1, 10));
			// encounter 4
			REQUIRE(60 == fys::util::RandomGenerator::generateInRange(0, 100));
			REQUIRE(2 == fys::util::RandomGenerator::generateInRange(1, 10));
		} // End section : test seed hard

		SECTION("test generate contender Easy") { // seed 2 80 10 28 8
			FightingPitAnnouncer fpa(cml);
			fpa.setCreatorUserName(" ");
			fpa.setCreatorUserToken(" ");
			fpa.setCreatorTeamParty(getPartyTeam(" "));
			fpa.setDifficulty(FightingPit::EASY);
			fpa.setEncounterType(FightingPitAnnouncer::EncounterType::RANDOM);
			auto fp = fpa.buildFightingPit(ctx, "WS00");

			REQUIRE(2 == FightingPitAnnouncer::getPitContenders(fp).getNumberContender());

			REQUIRE("Slime" == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(0)->getContenderScripting()->getContenderName());
			REQUIRE(0 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(0)->getContenderScripting()->getContenderId());
			REQUIRE(10 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(0)->getContenderScripting()->getLevel());

			REQUIRE("Sampy" == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(1)->getContenderScripting()->getContenderName());
			REQUIRE(1 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(1)->getContenderScripting()->getContenderId());
			REQUIRE(8 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(1)->getContenderScripting()->getLevel());

		} // End section : test generate contender Easy

		SECTION("test generate contender Medium") { // seed 3 80 10 18 8 78 6
			FightingPitAnnouncer fpa(cml);
			fpa.setCreatorUserToken(" ");
			fpa.setCreatorUserName(" ");
			fpa.setCreatorTeamParty(getPartyTeam(" "));
			fpa.setDifficulty(FightingPit::MEDIUM);
			fpa.setEncounterType(FightingPitAnnouncer::EncounterType::RANDOM);
			auto fp = fpa.buildFightingPit(ctx, "WS00");

			REQUIRE(3 == FightingPitAnnouncer::getPitContenders(fp).getNumberContender());

			REQUIRE("Slime" == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(0)->getContenderScripting()->getContenderName());
			REQUIRE(0 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(0)->getContenderScripting()->getContenderId());
			REQUIRE(10 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(0)->getContenderScripting()->getLevel());

			REQUIRE("Sampy" == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(1)->getContenderScripting()->getContenderName());
			REQUIRE(1 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(1)->getContenderScripting()->getContenderId());
			REQUIRE(8 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(1)->getContenderScripting()->getLevel());

			REQUIRE("Slime" == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(2)->getContenderScripting()->getContenderName());
			REQUIRE(2 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(2)->getContenderScripting()->getContenderId());
			REQUIRE(6 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(2)->getContenderScripting()->getLevel());

		} // End section : test generate contender Medium

		SECTION("test generate contender Hard") { // seed 4 80 10 18 8 78 6 60 2
			FightingPitAnnouncer fpa(cml);
			fpa.setCreatorUserToken(" ");
			fpa.setCreatorUserName(" ");
			fpa.setCreatorTeamParty(getPartyTeam(" "));
			fpa.setDifficulty(FightingPit::HARD);
			fpa.setEncounterType(FightingPitAnnouncer::EncounterType::RANDOM);
			auto fp = fpa.buildFightingPit(ctx, "WS00");

			REQUIRE(4 == FightingPitAnnouncer::getPitContenders(fp).getNumberContender());

			REQUIRE("Slime" == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(0)->getContenderScripting()->getContenderName());
			REQUIRE(0 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(0)->getContenderScripting()->getContenderId());
			REQUIRE(10 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(0)->getContenderScripting()->getLevel());

			REQUIRE("Sampy" == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(1)->getContenderScripting()->getContenderName());
			REQUIRE(1 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(1)->getContenderScripting()->getContenderId());
			REQUIRE(8 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(1)->getContenderScripting()->getLevel());

			REQUIRE("Slime" == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(2)->getContenderScripting()->getContenderName());
			REQUIRE(2 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(2)->getContenderScripting()->getContenderId());
			REQUIRE(6 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(2)->getContenderScripting()->getLevel());

			REQUIRE("Sampy" == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(3)->getContenderScripting()->getContenderName());
			REQUIRE(3 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(3)->getContenderScripting()->getContenderId());
			REQUIRE(2 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(3)->getContenderScripting()->getLevel());

		} // End section : test generate contender Hard

	} // End section : RNG test 42

	SECTION("test seed 1337") {
		std::shared_ptr<std::mt19937> mt = std::make_shared<std::mt19937>(1337);
		fseamMock->dupeReturn<FSeam::RandomGenerator::get>(mt);

		SECTION("test seed Easy") {
			REQUIRE(2 == fys::util::RandomGenerator::generateInRange(1, 4));
			// encounter 1
			REQUIRE(56 == fys::util::RandomGenerator::generateInRange(0, 100));
			REQUIRE(2 == fys::util::RandomGenerator::generateInRange(1, 10));
			// encounter 2
			REQUIRE(21 == fys::util::RandomGenerator::generateInRange(0, 100));
			REQUIRE(3 == fys::util::RandomGenerator::generateInRange(1, 10));
		} // End section : test seed Easy

		SECTION("test generate contender Easy") { // seed 2 56 2 21 3
			FightingPitAnnouncer fpa(cml);
			fpa.setCreatorUserToken(" ");
			fpa.setCreatorUserName(" ");
			fpa.setCreatorTeamParty(getPartyTeam(" "));
			fpa.setDifficulty(FightingPit::EASY);
			fpa.setEncounterType(FightingPitAnnouncer::EncounterType::RANDOM);
			auto fp = fpa.buildFightingPit(ctx, "WS00");

			REQUIRE(2 == FightingPitAnnouncer::getPitContenders(fp).getNumberContender());

			REQUIRE("Sampy" == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(0)->getContenderScripting()->getContenderName());
			REQUIRE(0 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(0)->getContenderScripting()->getContenderId());
			REQUIRE(2 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(0)->getContenderScripting()->getLevel());

			REQUIRE("Sampy" == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(1)->getContenderScripting()->getContenderName());
			REQUIRE(1 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(1)->getContenderScripting()->getContenderId());
			REQUIRE(3 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(1)->getContenderScripting()->getLevel());

			SECTION("test ordering turn setup") {
				REQUIRE(8 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(0)->getStatus().initialSpeed);
				REQUIRE(HexagonSide::Orientation::B_S == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(0)->getHexagonSideOrient());
				REQUIRE(8 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(1)->getStatus().initialSpeed);
				REQUIRE(HexagonSide::Orientation::B_S == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(1)->getHexagonSideOrient());

				const auto members = FightingPitAnnouncer::getPartyTeams(fp).getMembersBySide(HexagonSide::Orientation::B_S);
				REQUIRE(4 == members.size());
				REQUIRE(1 == members.at(0)->getId());
				REQUIRE(3 == members.at(0)->getStatus().initialSpeed);
				REQUIRE(2 == members.at(1)->getId());
				REQUIRE(5 == members.at(1)->getStatus().initialSpeed);
				REQUIRE(3 == members.at(2)->getId());
				REQUIRE(10 == members.at(2)->getStatus().initialSpeed);
				REQUIRE(4 == members.at(3)->getId());
				REQUIRE(20 == members.at(3)->getStatus().initialSpeed);

				SECTION("test ordering") {

					using namespace std::chrono_literals;
					auto& order = FightingPitAnnouncer::getSideBattleForSide(fp, HexagonSide::Orientation::B_S);
					REQUIRE(HexagonSide::Orientation::B_S == order.getSide());

					const auto elem1 = order.getCurrentParticipantTurn(std::chrono::system_clock::now(), 0ms);
					REQUIRE(data::PriorityElem{4, 20, data::PARTY_MEMBER} == elem1);
					REQUIRE(20 == elem1.speed);
					std::this_thread::sleep_for(10ms);

					const auto elem2 = order.getCurrentParticipantTurn(std::chrono::system_clock::now(), 0ms);
					REQUIRE(data::PriorityElem{3, 10, data::PARTY_MEMBER} == elem2);
					REQUIRE(10 == elem2.speed);
					std::this_thread::sleep_for(10ms);

					const auto elem3 = order.getCurrentParticipantTurn(std::chrono::system_clock::now(), 0ms);
					REQUIRE(data::PriorityElem{1, 8, data::CONTENDER} == elem3);
					REQUIRE(8 == elem3.speed);
					std::this_thread::sleep_for(10ms);

					const auto elem4 = order.getCurrentParticipantTurn(std::chrono::system_clock::now(), 0ms);
					REQUIRE(data::PriorityElem{0, 8, data::CONTENDER} == elem4);
					REQUIRE(8 == elem4.speed);
					std::this_thread::sleep_for(10ms);

					const auto elem5 = order.getCurrentParticipantTurn(std::chrono::system_clock::now(), 0ms);
					REQUIRE(data::PriorityElem{2, 5, data::PARTY_MEMBER} == elem5);
					REQUIRE(5 == elem5.speed);
					std::this_thread::sleep_for(10ms);

					const auto& elem = order.getCurrentParticipantTurn(std::chrono::system_clock::now(), 0ms);
					REQUIRE(data::PriorityElem{1, 4, data::PARTY_MEMBER} == elem);
					REQUIRE(3 == elem.speed);
					std::this_thread::sleep_for(10ms);

				} // End section : test ordering

			}

		} // End section : test generate contender Easy

	} // End section : RNG test 1337

	FSeam::MockVerifier::cleanUp();
} // End TestCase : FightingPitAnnouncer test



#pragma clang diagnostic pop