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
#include <map>

#include <chaiscript/chaiscript.hpp>

#include <fightingPit/contender/PitContenders.hh>
#include <fightingPit/team/AllyPartyTeams.hh>
#include <fightingPit/team/TeamMember.hh>
#include <CmlCopy.hh>

#include <ChaiRegister.hh>

namespace {
std::string
getLocalPathStorage()
{
	std::string file_path = __FILE__;
	std::string dir_path = file_path.substr(0, file_path.rfind('\\'));
	if (dir_path.size() == file_path.size())
		dir_path = file_path.substr(0, file_path.rfind('/'));
	return dir_path + "/testCopyTo";
}

std::string
getCopyPathStorage()
{
	std::string file_path = __FILE__;
	std::string dir_path = file_path.substr(0, file_path.rfind('\\'));
	if (dir_path.size() == file_path.size())
		dir_path = file_path.substr(0, file_path.rfind('/'));
	return dir_path + "/scripts_lnk";
}
}

TEST_CASE("Test register/load player", "[service][arena][script_test]")
{

	fys::cache::CmlCopy ccpy(getLocalPathStorage(), getCopyPathStorage());
	std::filesystem::path baseCache = getLocalPathStorage();

	fys::arena::PitContenders pc;
	fys::arena::AllyPartyTeams apt;
	fys::arena::FightingPitLayout layout(pc, apt);
	auto chai = fys::arena::ChaiRegister::createChaiInstance(pc, apt, layout);
	fys::arena::ChaiRegister::registerBaseActions(*chai, ccpy);

	try {
		REQUIRE(chai->eval<bool>(R"(
        var objs = get_objects();
        objs.count("ally_actions") == 1
        )"));
	}
	catch (std::exception& ex) {
		SPDLOG_ERROR("{}", ex.what());
		FAIL("Shouldn't fail here");
	}

	SECTION("Test copy cml") {

		auto content = ccpy.findInCache("arena:actions:damage:slash.chai");

		REQUIRE(std::filesystem::exists(baseCache));
		REQUIRE(std::filesystem::exists(baseCache / "arena"));
		REQUIRE(std::filesystem::exists(baseCache / "arena" / "actions"));
		REQUIRE(std::filesystem::exists(baseCache / "arena" / "actions" / "damage"));
		REQUIRE(std::filesystem::exists(baseCache / "arena" / "actions" / "damage" / "slash.chai"));
		REQUIRE_FALSE(content.empty());

	} // End section : Test copy cml

	SECTION("simple test load player actions") {

		fys::arena::PartyTeam partyTeam("FyS");
		fys::arena::TeamMemberSPtr tm1 = std::make_shared<fys::arena::TeamMember>(partyTeam.getUserName(), "fyston1");
		tm1->setId(0);
		tm1->addDoableAction("arena:actions:damage:slash.chai", 1);
		partyTeam.addTeamMember(std::move(tm1));

		fys::arena::ChaiRegister::loadAndRegisterActionPartyTeam(*chai, ccpy, partyTeam);

		try {
			REQUIRE(chai->eval<bool>(R"(1 == ally_actions.count("FyS_fyston1");)"));
			REQUIRE(chai->eval<bool>(R"(1 == ally_actions["FyS_fyston1"].count("slash");)"));
		}
		catch (std::exception& ex) {
			SPDLOG_ERROR("{}", ex.what());
			FAIL("Shouldn't fail here");
		}

		SECTION("simple test register same partyteam with loaded action") {

			fys::arena::TeamMemberSPtr tm2 = std::make_shared<fys::arena::TeamMember>(partyTeam.getUserName(), "fyston2");
			tm2->setId(1);
			tm2->addDoableAction("arena:actions:damage:slash.chai", 1);
			partyTeam.addTeamMember(std::move(tm2));

			fys::arena::ChaiRegister::loadAndRegisterActionPartyTeam(*chai, ccpy, partyTeam);

			try {
				REQUIRE(chai->eval<bool>(R"(1 == ally_actions.count("FyS_fyston1");)"));
				REQUIRE(chai->eval<bool>(R"(1 == ally_actions["FyS_fyston1"].count("slash");)"));

				REQUIRE(chai->eval<bool>(R"(1 == ally_actions.count("FyS_fyston2");)"));
				REQUIRE(chai->eval<bool>(R"(1 == ally_actions["FyS_fyston2"].count("slash");)"));
			}
			catch (std::exception& ex) {
				SPDLOG_ERROR("{}", ex.what());
				FAIL("Shouldn't fail here");
			}

		} // End section : Test register loaded action

		SECTION("simple test register new partyteam with loaded action") {

			fys::arena::PartyTeam partyTeam2("Free");
			fys::arena::TeamMemberSPtr tm21 = std::make_shared<fys::arena::TeamMember>(partyTeam2.getUserName(), "fyston1");
			tm21->setId(2);
			tm21->addDoableAction("arena:actions:damage:slash.chai", 1);
			partyTeam2.addTeamMember(std::move(tm21));
			fys::arena::TeamMemberSPtr tm22 = std::make_shared<fys::arena::TeamMember>(partyTeam2.getUserName(), "fyston2");
			tm22->setId(3);
			tm22->addDoableAction("arena:actions:damage:slash.chai", 1);
			partyTeam2.addTeamMember(std::move(tm22));

			fys::arena::ChaiRegister::loadAndRegisterActionPartyTeam(*chai, ccpy, partyTeam2);

			try {
				REQUIRE(chai->eval<bool>(R"(1 == ally_actions.count("Free_fyston1");)"));
				REQUIRE(chai->eval<bool>(R"(1 == ally_actions["Free_fyston1"].count("slash");)"));

				REQUIRE(chai->eval<bool>(R"(1 == ally_actions.count("Free_fyston2");)"));
				REQUIRE(chai->eval<bool>(R"(1 == ally_actions["Free_fyston2"].count("slash");)"));
			}
			catch (std::exception& ex) {
				SPDLOG_ERROR("{}", ex.what());
				FAIL("Shouldn't fail here");
			}

		} // End section : simple test register new partyteam with loaded action

	} // End section : Test load player actions

	std::filesystem::remove_all(baseCache);

} // End TestCase : Test register/load player
