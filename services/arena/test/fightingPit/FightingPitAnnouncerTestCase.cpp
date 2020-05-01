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
#include <fightingPit/contender/FightingContender.hh>
#include <fightingPit/contender/ContenderScripting.hh>
#include <RandomGenerator.hh>
#include "TestType.hh"

using namespace fys::arena;

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

TEST_CASE("FightingPitAnnouncerTestCase", "[service][arena]")
{
	auto fseamMock = FSeam::getDefault<fys::util::RandomGenerator>();
	auto cml = CmlBase(getLocalPathStorage());
	EncounterContext ctx;
	ctx._rangeEncounterPerZone["WS00"] = {
			EncounterContext::RngRange(1, 4), // ez
			EncounterContext::RngRange(2, 4), // medium
			EncounterContext::RngRange(3, 5)  // hard
	};
	ctx._contendersPerZone["WS00"] = {
			EncounterContext::EncounterDesc{
					"arena:contenders:Sampy.chai", 3,
					EncounterContext::ChanceArray{60, 60, 60},
					EncounterContext::RngRange(1u, 10u)
			},
			EncounterContext::EncounterDesc{
					"arena:contenders:Slime.chai", 3,
					EncounterContext::ChanceArray{40, 40, 40},
					EncounterContext::RngRange(1u, 10u)
			}
	};

	SECTION("test wrong range encounter") {
		FightingPitAnnouncer fpa(cml);
		fpa.setCreatorUserToken(" ");
		fpa.setCreatorUserName(" ");
		fpa.setCreatorTeamParty(getPartyTeam(" "));
		fpa.setDifficulty(FightingPit::MEDIUM);
		fpa.setEncounterType(FightingPitAnnouncer::EncounterType::RANDOM);
		REQUIRE(nullptr == fpa.buildFightingPit(ctx, "WS42"));
	} // End section : test wrong range encounter

	SECTION("test no creator userName") {
		FightingPitAnnouncer fpa(cml);
		fpa.setCreatorUserToken(" ");
		// fpa.setCreatorUserName(); !!NOT SET
		fpa.setCreatorTeamParty(getPartyTeam(" "));
		fpa.setDifficulty(FightingPit::MEDIUM);
		fpa.setEncounterType(FightingPitAnnouncer::EncounterType::RANDOM);
		REQUIRE(nullptr == fpa.buildFightingPit(ctx, "WS00"));
	} // End section : test no creator userName

	SECTION("test no creator userToken") {
		FightingPitAnnouncer fpa(cml);
		fpa.setCreatorUserName(" ");
		// fpa.setCreatorUserToken(); !!NOT SET
		fpa.setCreatorTeamParty(getPartyTeam(" "));
		fpa.setDifficulty(FightingPit::MEDIUM);
		fpa.setEncounterType(FightingPitAnnouncer::EncounterType::RANDOM);
		REQUIRE(nullptr == fpa.buildFightingPit(ctx, "WS00"));
	} // End section : test no creator userName

	SECTION("test invalid action name registered") {

		std::shared_ptr<std::mt19937> mt = std::make_shared<std::mt19937>(42);
		fseamMock->dupeReturn<FSeam::RandomGenerator::get>(mt);

		FightingPitAnnouncer fpa(cml);
		fpa.setCreatorUserToken(" ");
		fpa.setCreatorUserName(" ");
		fpa.setCreatorTeamParty(getPartyTeam(" "));
		fpa.setDifficulty(FightingPit::HARD);
		fpa.setEncounterType(FightingPitAnnouncer::EncounterType::RANDOM);
		fpa.addActionToOneMember(0, "arena", 5);
		auto fp = fpa.buildFightingPit(ctx, "WS00");

	} // End section : test invalid action name registered

	SECTION("Test Side Setup") {
		std::shared_ptr<std::mt19937> mt = std::make_shared<std::mt19937>(42);
		fseamMock->dupeReturn<FSeam::RandomGenerator::get>(mt);
		FightingPitAnnouncer fpa(cml);
		fpa.setCreatorUserName(" ");
		fpa.setCreatorUserToken(" ");
		fpa.setCreatorTeamParty(getPartyTeam(" "));
		fpa.setDifficulty(FightingPit::EASY);
		fpa.setEncounterType(FightingPitAnnouncer::EncounterType::RANDOM);
		fpa.addActionToOneMember(0, "arena:actions:damage:slash.chai", 5);
		auto fp = fpa.buildFightingPit(ctx, "WS00");

		const auto &sides = FightingPitAnnouncer::getSideVector(fp);
		REQUIRE(18 == sides.size());

		REQUIRE(HexagonSide::Orientation::A_N == sides[static_cast<uint>(HexagonSide::Orientation::A_N)].getSide());
		REQUIRE(HexagonSide::Orientation::A_NE == sides[static_cast<uint>(HexagonSide::Orientation::A_NE)].getSide());
		REQUIRE(HexagonSide::Orientation::A_NW == sides[static_cast<uint>(HexagonSide::Orientation::A_NW)].getSide());
		REQUIRE(HexagonSide::Orientation::A_S == sides[static_cast<uint>(HexagonSide::Orientation::A_S)].getSide());
		REQUIRE(HexagonSide::Orientation::A_SE == sides[static_cast<uint>(HexagonSide::Orientation::A_SE)].getSide());
		REQUIRE(HexagonSide::Orientation::A_SW == sides[static_cast<uint>(HexagonSide::Orientation::A_SW)].getSide());

		REQUIRE(HexagonSide::Orientation::B_N == sides[static_cast<uint>(HexagonSide::Orientation::B_N)].getSide());
		REQUIRE(HexagonSide::Orientation::B_NE == sides[static_cast<uint>(HexagonSide::Orientation::B_NE)].getSide());
		REQUIRE(HexagonSide::Orientation::B_NW == sides[static_cast<uint>(HexagonSide::Orientation::B_NW)].getSide());
		REQUIRE(HexagonSide::Orientation::B_S == sides[static_cast<uint>(HexagonSide::Orientation::B_S)].getSide());
		REQUIRE(HexagonSide::Orientation::B_SE == sides[static_cast<uint>(HexagonSide::Orientation::B_SE)].getSide());
		REQUIRE(HexagonSide::Orientation::B_SW == sides[static_cast<uint>(HexagonSide::Orientation::B_SW)].getSide());

		REQUIRE(HexagonSide::Orientation::C_N == sides[static_cast<uint>(HexagonSide::Orientation::C_N)].getSide());
		REQUIRE(HexagonSide::Orientation::C_NE == sides[static_cast<uint>(HexagonSide::Orientation::C_NE)].getSide());
		REQUIRE(HexagonSide::Orientation::C_NW == sides[static_cast<uint>(HexagonSide::Orientation::C_NW)].getSide());
		REQUIRE(HexagonSide::Orientation::C_S == sides[static_cast<uint>(HexagonSide::Orientation::C_S)].getSide());
		REQUIRE(HexagonSide::Orientation::C_SE == sides[static_cast<uint>(HexagonSide::Orientation::C_SE)].getSide());
		REQUIRE(HexagonSide::Orientation::C_SW == sides[static_cast<uint>(HexagonSide::Orientation::C_SW)].getSide());
	} // End section : Test Side Setup

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
			fpa.addActionToOneMember(0, "arena:actions:damage:slash.chai", 5);
			auto fp = fpa.buildFightingPit(ctx, "WS00");

			REQUIRE_FALSE(fp->isBattleOver());
			REQUIRE(fp->isJoinable());
			REQUIRE_THROWS(fp->setPlayerReadiness("NotExisting")); // test if the fp throws in this case

			REQUIRE(2 == FightingPitAnnouncer::getPitContenders(fp).getNumberContender());

			REQUIRE("Slime" == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(0)->getName());
			REQUIRE(0 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(0)->getId());
			REQUIRE(10 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(0)->getContenderScripting()->getLevel());

			REQUIRE("Sampy" == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(1)->getName());
			REQUIRE(1 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(1)->getContenderScripting()->getContenderId());
			REQUIRE(8 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(1)->getContenderScripting()->getLevel());

		} // End section : test generate contender Easy

		SECTION("test generate contender Medium") { // seed 3 80 10 18 8 78 6
			FightingPitAnnouncer fpa(cml);
			fpa.setCreatorUserToken(" ");
			fpa.setCreatorUserName(" ");
			fpa.setCreatorTeamParty(getPartyTeam(" "));
			fpa.setJoinDisabled(true);
			fpa.setDifficulty(FightingPit::MEDIUM);
			fpa.setEncounterType(FightingPitAnnouncer::EncounterType::RANDOM);
			fpa.addActionToOneMember(0, "arena:actions:damage:slash.chai", 5);
			auto fp = fpa.buildFightingPit(ctx, "WS00");

			REQUIRE(4 == FightingPitAnnouncer::getPartyTeams(fp).getPartyTeamOfPlayer(" ").getTeamMembers().size());
			REQUIRE_FALSE(fp->isJoinable());

			REQUIRE(3 == FightingPitAnnouncer::getPitContenders(fp).getNumberContender());

			REQUIRE("Slime" == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(0)->getName());
			REQUIRE(0 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(0)->getContenderScripting()->getContenderId());
			REQUIRE(10 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(0)->getContenderScripting()->getLevel());

			REQUIRE("Sampy" == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(1)->getName());
			REQUIRE(1 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(1)->getContenderScripting()->getContenderId());
			REQUIRE(8 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(1)->getContenderScripting()->getLevel());

			REQUIRE("Slime" == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(2)->getName());
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
			fpa.addActionToOneMember(0, "arena:actions:damage:slash.chai", 5);
			auto fp = fpa.buildFightingPit(ctx, "WS00");

			REQUIRE(4 == FightingPitAnnouncer::getPitContenders(fp).getNumberContender());
			REQUIRE(fp->isJoinable());

			REQUIRE("Slime" == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(0)->getName());
			REQUIRE(0 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(0)->getContenderScripting()->getContenderId());
			REQUIRE(10 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(0)->getContenderScripting()->getLevel());

			REQUIRE("Sampy" == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(1)->getName());
			REQUIRE(1 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(1)->getContenderScripting()->getContenderId());
			REQUIRE(8 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(1)->getContenderScripting()->getLevel());

			REQUIRE("Slime" == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(2)->getName());
			REQUIRE(2 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(2)->getContenderScripting()->getContenderId());
			REQUIRE(6 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(2)->getContenderScripting()->getLevel());

			REQUIRE("Sampy" == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(3)->getName());
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
			fpa.addActionToOneMember(0, "arena:actions:damage:slash.chai", 5);
			auto fp = fpa.buildFightingPit(ctx, "WS00");

			REQUIRE(2 == FightingPitAnnouncer::getPitContenders(fp).getNumberContender());

			REQUIRE("Sampy" == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(0)->getName());
			REQUIRE(0 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(0)->getContenderScripting()->getContenderId());
			REQUIRE(2 == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(0)->getContenderScripting()->getLevel());

			REQUIRE("Sampy" == FightingPitAnnouncer::getPitContenders(fp).getFightingContender(1)->getName());
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

TEST_CASE("FightingPitAnnouncerTestCase test reward", "[service][arena]")
{
	auto fseamMock = FSeam::getDefault<fys::util::RandomGenerator>();
	auto cml = CmlBase(getLocalPathStorage());
	EncounterContext ctx;
	ctx._rangeEncounterPerZone["WS00"] = {
			EncounterContext::RngRange(1, 4), // ez
			EncounterContext::RngRange(1, 4), // medium
			EncounterContext::RngRange(1, 4)  // hard
	};
	ctx._contendersPerZone["WS00"] = {
			EncounterContext::EncounterDesc{
					"arena:contenders:Sampy.chai", 3,
					EncounterContext::ChanceArray{60, 60, 60},
					EncounterContext::RngRange(1u, 10u)
			},
			EncounterContext::EncounterDesc{
					"arena:contenders:Slime.chai", 3,
					EncounterContext::ChanceArray{40, 40, 40},
					EncounterContext::RngRange(1u, 10u)
			}
	};
	ctx._rewardDescPerContender["Sampy"] = EncounterContext::RewardEncounterDesc{
			std::array<EncounterContext::RngRange, 3>{
					EncounterContext::RngRange(3, 3),
					EncounterContext::RngRange(2, 2),
					EncounterContext::RngRange(6, 6)
			},
			{
					{"DrPepper", EncounterContext::ChanceArray{50, 50, 50}},
					{"EsCaliBur", EncounterContext::ChanceArray{50, 50, 50}},
			}
	};
	std::shared_ptr<std::mt19937> mt = std::make_shared<std::mt19937>(42);
	fseamMock->dupeReturn<FSeam::RandomGenerator::get>(mt);

	SECTION("test seed") {
		REQUIRE(2 == fys::util::RandomGenerator::generateInRange(1, 4));
		// encounter 1
		REQUIRE(80 == fys::util::RandomGenerator::generateInRange(0, 100));
		REQUIRE(10 == fys::util::RandomGenerator::generateInRange(1, 10));
		// encounter 2
		REQUIRE(18 == fys::util::RandomGenerator::generateInRange(0, 100));
		REQUIRE(8 == fys::util::RandomGenerator::generateInRange(1, 10));
		// Reward
		REQUIRE(3 == fys::util::RandomGenerator::generateInRange(3, 3));     // number of reward
		REQUIRE(60 == fys::util::RandomGenerator::generateInRange(0, 100));  // first  reward is EsCaliBur
		REQUIRE(60 == fys::util::RandomGenerator::generateInRange(0, 100));  // second reward is EsCaliBur
		REQUIRE(15 == fys::util::RandomGenerator::generateInRange(0, 100));  // third  reward is DrPepper

	} // End section : test seed ez

	SECTION("Test one type reward generation") {
		FightingPitAnnouncer fpa(cml);
		fpa.setCreatorUserName(" ");
		fpa.setCreatorUserToken(" ");
		fpa.setCreatorTeamParty(getPartyTeam(" "));
		fpa.setDifficulty(FightingPit::MEDIUM);
		fpa.setEncounterType(FightingPitAnnouncer::EncounterType::RANDOM);
		fpa.addActionToOneMember(0, "arena:actions:damage:slash.chai", 5);
		auto fp = fpa.buildFightingPit(ctx, "WS00");

		REQUIRE(1 == FightingPitAnnouncer::getReward(fp).keys.size());
		REQUIRE(1 == FightingPitAnnouncer::getReward(fp).quantity.size());
		REQUIRE("EsCaliBur" == FightingPitAnnouncer::getReward(fp).keys.at(0));
		REQUIRE(2 == FightingPitAnnouncer::getReward(fp).quantity.at(0));

	} // End section : Test one type reward generation

	SECTION("Test different reward generation") {
		FightingPitAnnouncer fpa(cml);
		fpa.setCreatorUserName(" ");
		fpa.setCreatorUserToken(" ");
		fpa.setCreatorTeamParty(getPartyTeam(" "));
		fpa.setDifficulty(FightingPit::EASY);
		fpa.setEncounterType(FightingPitAnnouncer::EncounterType::RANDOM);
		fpa.addActionToOneMember(0, "arena:actions:damage:slash.chai", 5);
		auto fp = fpa.buildFightingPit(ctx, "WS00");

		REQUIRE(2 == FightingPitAnnouncer::getReward(fp).keys.size()); // 2 different reward
		REQUIRE(2 == FightingPitAnnouncer::getReward(fp).quantity.size());
		REQUIRE("DrPepper" == FightingPitAnnouncer::getReward(fp).keys.at(0));
		REQUIRE(1 == FightingPitAnnouncer::getReward(fp).quantity.at(0));
		REQUIRE("EsCaliBur" == FightingPitAnnouncer::getReward(fp).keys.at(1));
		REQUIRE(2 == FightingPitAnnouncer::getReward(fp).quantity.at(1));
	} // End section : Test different reward generation

	SECTION("test seed for 'Multiple Monster of same type add up'") {
		REQUIRE(2 == fys::util::RandomGenerator::generateInRange(1, 4));
		// encounter 1
		REQUIRE(80 == fys::util::RandomGenerator::generateInRange(0, 100)); // Slime is selected
		REQUIRE(10 == fys::util::RandomGenerator::generateInRange(1, 10));  // level Slime is 10
		// encounter 2
		REQUIRE(18 == fys::util::RandomGenerator::generateInRange(0, 100)); // Sampy is selected
		REQUIRE(8 == fys::util::RandomGenerator::generateInRange(1, 10));   // level Sampy is 8

		// Reward Slime
		REQUIRE(3 == fys::util::RandomGenerator::generateInRange(3, 3));     // number of reward for Slime
		REQUIRE(60 == fys::util::RandomGenerator::generateInRange(0, 100));  // first  reward is TriForce
		REQUIRE(60 == fys::util::RandomGenerator::generateInRange(0, 100));  // second reward is TriForce
		REQUIRE(15 == fys::util::RandomGenerator::generateInRange(0, 100));  // third  reward is TriForce
		// Reward Sampy
		REQUIRE(6 == fys::util::RandomGenerator::generateInRange(6, 6));     // number of reward for Sampy
		REQUIRE(15 == fys::util::RandomGenerator::generateInRange(0, 100));  // first  reward is DrPepper
		REQUIRE(10 == fys::util::RandomGenerator::generateInRange(0, 100));  // second reward is DrPepper
		REQUIRE(5 == fys::util::RandomGenerator::generateInRange(0, 100));   // third  reward is DrPepper
		REQUIRE(46 == fys::util::RandomGenerator::generateInRange(0, 100));  // fourth reward is DrPepper
		REQUIRE(87 == fys::util::RandomGenerator::generateInRange(0, 100));  // fith   reward is EsCaliBur
		REQUIRE(33 == fys::util::RandomGenerator::generateInRange(0, 100));  // sixth  reward is DrPepper

	} // End section : test seed ez

	SECTION("Multiple Monster of same type add up") {

		ctx._rewardDescPerContender["Slime"] = EncounterContext::RewardEncounterDesc{
				std::array<EncounterContext::RngRange, 3>{
						EncounterContext::RngRange(3, 3),
						EncounterContext::RngRange(2, 2),
						EncounterContext::RngRange(3, 3)
				},
				{
						{"TriForce", EncounterContext::ChanceArray{100, 100, 100}}
				}
		};
		FightingPitAnnouncer fpa(cml);
		fpa.setCreatorUserName(" ");
		fpa.setCreatorUserToken(" ");
		fpa.setCreatorTeamParty(getPartyTeam(" "));
		fpa.setDifficulty(FightingPit::HARD);
		fpa.setEncounterType(FightingPitAnnouncer::EncounterType::RANDOM);
		fpa.addActionToOneMember(0, "arena:actions:damage:slash.chai", 5);
		auto fp = fpa.buildFightingPit(ctx, "WS00");

		// 3 different rewards
		REQUIRE(3 == FightingPitAnnouncer::getReward(fp).keys.size());
		REQUIRE(3 == FightingPitAnnouncer::getReward(fp).quantity.size());

		REQUIRE("DrPepper" == FightingPitAnnouncer::getReward(fp).keys.at(0));
		REQUIRE(5 == FightingPitAnnouncer::getReward(fp).quantity.at(0));
		REQUIRE("EsCaliBur" == FightingPitAnnouncer::getReward(fp).keys.at(1));
		REQUIRE(1 == FightingPitAnnouncer::getReward(fp).quantity.at(1));
		REQUIRE("TriForce" == FightingPitAnnouncer::getReward(fp).keys.at(2));
		REQUIRE(3 == FightingPitAnnouncer::getReward(fp).quantity.at(2));

	} // End section : Multiple Monster of same type add up

} // End TestCase :  FightingPitAnnouncer test reward


#pragma clang diagnostic pop