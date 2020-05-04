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
#include <memory>

#include <chaiscript/chaiscript.hpp>

#include <fightingPit/contender/FightingContender.hh>
#include <fightingPit/contender/PitContenders.hh>
#include <fightingPit/contender/ContenderScripting.hh>
#include <fightingPit/team/AllyPartyTeams.hh>
#include <fightingPit/team/TeamMember.hh>

#include <ChaiRegister.hh>
#include <CmlCopy.hh>

namespace {
std::string
getPathSampyChaiScript()
{
	std::string file_path = __FILE__;
	std::string dir_path = file_path.substr(0, file_path.rfind('\\'));
	if (dir_path.size() == file_path.size())
		dir_path = file_path.substr(0, file_path.rfind('/'));
	return dir_path + "/scripts_lnk/arena/contenders/Sampy.chai";
}

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

TEST_CASE("test heal chaiscript", "[service][arena][script_test]")
{

	fys::cache::CmlCopy ccpy(getLocalPathStorage(), getCopyPathStorage());
	std::filesystem::path baseCache = getLocalPathStorage();

	fys::arena::PitContenders pc;
	fys::arena::AllyPartyTeams apt;
	fys::arena::FightingPitLayout layout(pc, apt);
	auto chai = fys::arena::ChaiRegister::createChaiInstance(pc, apt, layout);
	fys::arena::ChaiRegister::registerBaseActions(*chai, ccpy);

	fys::arena::ContenderScriptingUPtr sampy = std::make_unique<fys::arena::ContenderScripting>(*chai, 1);
	sampy->setContenderId(0u);
	sampy->setContenderName("Sampy");
	sampy->loadContenderScriptFromFile(getPathSampyChaiScript());

	auto fpc = std::make_shared<fys::arena::FightingContender>(std::move(sampy));
	REQUIRE(pc.addContender(fpc));

	SECTION("Test initialization contender") {
		REQUIRE(8 == pc.getFightingContender(0)->accessStatus().initialSpeed);
		REQUIRE(153 == pc.getFightingContender(0)->accessStatus().life.current);
		REQUIRE(153 == pc.getFightingContender(0)->accessStatus().life.total);
		REQUIRE(100 == pc.getFightingContender(0)->accessStatus().magicPoint.total);
		REQUIRE(100 == pc.getFightingContender(0)->accessStatus().magicPoint.total);
	}

	fys::arena::PartyTeam partyTeam("FyS");
	fys::arena::TeamMemberSPtr tm1 = std::make_shared<fys::arena::TeamMember>(partyTeam.getUserName(), "fyston1");
	auto& st = tm1->accessStatus();
	st.initialSpeed = 100;
	st.magicPoint = {1337, 1337};
	st.life = {42, 42};

	SECTION("test light_heal chaiscript") {

		tm1->addDoableAction("arena:actions:heal:light_heal.chai", 1);
		partyTeam.addTeamMember(std::move(tm1));

		fys::arena::ChaiRegister::loadAndRegisterActionPartyTeam(*chai, ccpy, partyTeam);

		SECTION("test heal") {

			pc.getFightingContender(0)->accessStatus().life.current = 100;

			try {
				chai->eval(R"(
var &contender = pitContenders.getFightingContender(0);
var s = light_heal(1);
s.execute(contender);
)");
			}
			catch (std::exception& ex) {
				SPDLOG_ERROR("{}", ex.what());
				FAIL("Chaiscript : Shouldn't fail here");
			}
			REQUIRE(133 == pc.getFightingContender(0)->accessStatus().life.current); // +33 life
			REQUIRE(153 == pc.getFightingContender(0)->accessStatus().life.total);

		} // End section : test heal

		SECTION("test over healing ") {

			pc.getFightingContender(0)->accessStatus().life.current = 140;

			try {
				chai->eval(R"(
var &contender = pitContenders.getFightingContender(0);
var s = light_heal(1);
s.execute(contender);
)");
			}
			catch (std::exception& ex) {
				SPDLOG_ERROR("{}", ex.what());
				FAIL("Chaiscript : Shouldn't fail here");
			}
			REQUIRE(153 == pc.getFightingContender(0)->accessStatus().life.current); // +33 life > go to max
			REQUIRE(153 == pc.getFightingContender(0)->accessStatus().life.total);

		} // End section : test over healing

	} // End section : test light_heal chaiscript

	std::filesystem::remove_all(baseCache);

} // End TestCase : test Slash chaiscript