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

#include <fightingPit/contender/fighting_contender.hh>
#include <fightingPit/contender/pit_contenders.hh>
#include <fightingPit/contender/contender_scripting.hh>
#include <fightingPit/team/ally_party_teams.hh>
#include <fightingPit/team/team_member.hh>

#include <chai_register.hh>
#include <CmlCopy.hh>
#include <fightingPit/fighting_pit_announcer.hh>

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

TEST_CASE("test damage chaiscript", "[service][arena][script_test]")
{

	fys::cache::CmlCopy ccpy(getLocalPathStorage(), getCopyPathStorage());
	std::filesystem::path baseCache = getLocalPathStorage();

	fys::arena::pit_contenders pc;
	fys::arena::ally_party_teams apt;
	fys::arena::fighting_pit_layout layout(pc, apt);
	auto chai = fys::arena::chai_register::make_chai_instance(pc, apt, layout);

	fys::arena::chai_register::register_base_actions(*chai, ccpy);

	fys::arena::ContenderScriptingUPtr sampy = std::make_unique<fys::arena::contender_scripting>(*chai, 1);
	sampy->set_contender_id(0u);
	sampy->set_contender_name("Sampy");
	sampy->load_contender_script_from_file(getPathSampyChaiScript());

	auto fpc = std::make_shared<fys::arena::fighting_contender>(std::move(sampy));
	REQUIRE(pc.addContender(fpc));

	SECTION("Test initialization contender") {
		REQUIRE(153 == fpc->access_status().life.current);
		REQUIRE(8 == pc.getFightingContender(0)->access_status().initial_speed);
		REQUIRE(153 == pc.getFightingContender(0)->access_status().life.current);
		REQUIRE(153 == pc.getFightingContender(0)->access_status().life.total);
		REQUIRE(100 == pc.getFightingContender(0)->access_status().magic_point.total);
		REQUIRE(100 == pc.getFightingContender(0)->access_status().magic_point.total);
	}

	fys::arena::party_team partyTeam("FyS");
	fys::arena::team_member_sptr tm1 = std::make_shared<fys::arena::team_member>(partyTeam.get_user_name(), "fyston1");
	auto& st = tm1->access_status();
	st.initial_speed = 100;
	st.magic_point = {1337, 1337};
	st.life = {42, 42};

	SECTION("test Slash chaiscript") {

		tm1->add_doable_action("arena:actions:damage:slash.chai", 1);
		partyTeam.add_team_member(std::move(tm1));

		fys::arena::chai_register::load_register_action_party_team(*chai, ccpy, partyTeam);

		SECTION("test damage") {

			try {
				chai->eval(R"(
var &contender = pitContenders.getFightingContender(0);
var s = slash(1);
s.execute(contender);
)");
			}
			catch (const chaiscript::exception::eval_error & ex) {
				SPDLOG_ERROR("{}", ex.pretty_print());
				FAIL("Chaiscript : Shouldn't fail here");
			}
			REQUIRE(120 == pc.getFightingContender(0)->access_status().life.current); // -33 life
			REQUIRE(153 == pc.getFightingContender(0)->access_status().life.total);
			REQUIRE(1 == pc.getFightingContender(0)->getStatus().alteration_after.size());
			auto& stat = pc.getFightingContender(0)->access_status();
			auto alt = stat.alteration_after.at(0);
			REQUIRE(2 == alt.getTurn());
			alt.process_alteration(stat);
			REQUIRE(1 == alt.getTurn());
		} // End section : test damage

		SECTION("test over damage") {

			pc.getFightingContender(0)->access_status().life.current = 10;

			try {
				chai->eval(R"(
var &contender = pitContenders.getFightingContender(0);
var s = slash(1);
s.execute(contender);
)");
			}
			catch (std::exception& ex) {
				SPDLOG_ERROR("{}", ex.what());
				FAIL("Chaiscript : Shouldn't fail here");
			}
			REQUIRE(0 == pc.getFightingContender(0)->access_status().life.current); // -33 life > 0
			REQUIRE(153 == pc.getFightingContender(0)->access_status().life.total);

		} // End section : test over damage

	} // End section : test Slash chaiscript

	SECTION("test Multi_Slash chaiscript") {

		tm1->add_doable_action("arena:actions:damage:multi_slash.chai", 1);
		partyTeam.add_team_member(std::move(tm1));

		fys::arena::ContenderScriptingUPtr sampy2 = std::make_unique<fys::arena::contender_scripting>(*chai, 1);
		sampy2->set_contender_id(1u);
		sampy2->set_contender_name("Sampy");
		sampy2->load_contender_script_from_file(getPathSampyChaiScript());

		auto fpc2 = std::make_shared<fys::arena::fighting_contender>(std::move(sampy2));
		REQUIRE(pc.addContender(fpc2));

		fys::arena::chai_register::load_register_action_party_team(*chai, ccpy, partyTeam);

		pc.getFightingContender(1)->access_status().life.current = 100;

		SECTION("test damage") {

			try {
				chai->eval(R"(
var &contenderStatus = pitContenders.getFightingContender(0);
var &contender2Status = pitContenders.getFightingContender(1);
var s = multi_slash(1);
var d = [];
d.push_back_ref(contenderStatus);
d.push_back_ref(contender2Status);
s.execute(d);
)");
			}
			catch (std::exception& ex) {
				SPDLOG_ERROR("{}", ex.what());
				FAIL("Chaiscript : Shouldn't fail here");
			}
			REQUIRE(123 == pc.getFightingContender(0)->access_status().life.current); // -33 life
			REQUIRE(153 == pc.getFightingContender(0)->access_status().life.total);

			REQUIRE(70 == pc.getFightingContender(1)->access_status().life.current); // -33 life
			REQUIRE(153 == pc.getFightingContender(1)->access_status().life.total);

		} // End section : test damage

		SECTION("test over damage") {

			pc.getFightingContender(0)->access_status().life.current = 10;

			try {
				chai->eval(R"(
var &contenderStatus = pitContenders.getFightingContender(0);
var &contender2Status = pitContenders.getFightingContender(1);
var s = multi_slash(1);
var d = [];
d.push_back_ref(contenderStatus);
d.push_back_ref(contender2Status);
s.execute(d);
)");
			}
			catch (std::exception& ex) {
				SPDLOG_ERROR("{}", ex.what());
				FAIL("Chaiscript : Shouldn't fail here");
			}

			REQUIRE(0 == pc.getFightingContender(0)->access_status().life.current); // -33 life > 0
			REQUIRE(153 == pc.getFightingContender(0)->access_status().life.total);

			REQUIRE(70 == pc.getFightingContender(1)->access_status().life.current); // -33 life
			REQUIRE(153 == pc.getFightingContender(1)->access_status().life.total);

		} // End section : test over damage

		std::filesystem::remove_all(baseCache);

	} // End section : test Multi_Slash chaiscript

} // End TestCase : test damage chaiscript