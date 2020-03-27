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


#ifndef FYS_ONLINE_TESTTYPE_HH
#define FYS_ONLINE_TESTTYPE_HH

#include <RandomGenerator.hh>
#include <FSeamMockData.hpp>
#include <fightingPit/team/PartyTeam.hh>
#include <fightingPit/team/TeamMember.hh>
#include <CmlKey.hh>
#include <Cml.hh>

class CmlBase : public fys::cache::Cml {
public:
	explicit CmlBase(std::string v)
			:fys::cache::Cml(std::filesystem::path(std::move(v))) { }

	void createFileInLocalStorage(const fys::cache::CmlKey& cmlKey) override { }
};

[[nodiscard]] static std::string
getLocalPathStorage()
{
	std::string file_path = __FILE__;
	std::string dir_path = file_path.substr(0, file_path.rfind('\\'));
	if (dir_path.size() == file_path.size())
		dir_path = file_path.substr(0, file_path.rfind('/'));
	return dir_path + "/../../scriptTests/scripts_lnk";
}

[[nodiscard]] static std::unique_ptr<fys::arena::PartyTeam>
getPartyTeam(const std::string& user) {
	auto team = std::make_unique<fys::arena::PartyTeam>(user);

	// Temporary hard coded party team
	auto tm1 = std::make_shared<fys::arena::TeamMember>(user, "Elvo");
	auto tm2 = std::make_shared<fys::arena::TeamMember>(user, "Mirael");
	auto tm3 = std::make_shared<fys::arena::TeamMember>(user, "Fyston");
	auto tm4 = std::make_shared<fys::arena::TeamMember>(user, "Simon");

	tm1->moveTeamMember(fys::arena::HexagonSide::Orientation::B_S, true);
	auto& s1 = tm1->accessStatus();
	s1.life.total = 100;
	s1.life.current = 100;
	s1.magicPoint.total = 20;
	s1.magicPoint.current = 20;
	s1.initialSpeed = 3;
	tm2->moveTeamMember(fys::arena::HexagonSide::Orientation::B_S, true);
	auto& s2 = tm2->accessStatus();
	s2.life.total = 200;
	s2.life.current = 200;
	s2.magicPoint.total = 0;
	s2.magicPoint.current = 0;
	s2.initialSpeed = 5;
	tm3->moveTeamMember(fys::arena::HexagonSide::Orientation::B_S, true);
	auto& s3 = tm3->accessStatus();
	s3.life.total = 550;
	s3.life.current = 550;
	s3.magicPoint.total = 10;
	s3.magicPoint.current = 10;
	s3.initialSpeed = 10;
	tm4->moveTeamMember(fys::arena::HexagonSide::Orientation::B_S, true);
	auto& s4 = tm4->accessStatus();
	s4.life.total = 140;
	s4.life.current = 140;
	s4.magicPoint.total = 10;
	s4.magicPoint.current = 10;
	s4.initialSpeed = 20;

	team->addTeamMember(std::move(tm1));
	team->addTeamMember(std::move(tm2));
	team->addTeamMember(std::move(tm3));
	team->addTeamMember(std::move(tm4));
	return team;
}

#endif //FYS_ONLINE_TESTTYPE_HH
