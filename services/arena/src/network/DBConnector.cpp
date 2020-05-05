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

//#include <mariadb++/account.hpp>
//#include <mariadb++/statement.hpp>
//#include <mariadb++/connection.hpp>
#include <ArenaServerContext.hh>
#include <network/DBConnector.hh>
#include <fightingPit/team/PartyTeam.hh>
#include <fightingPit/team/TeamMember.hh>

namespace fys::network {

DBConnector::DBConnector(const arena::ArenaServerContext& ctx)
{
//        _refDb = mariadb::account::create(ctx.getDbHost(), "Arena", "ArenaPWD", "fys", ctx.getDbPort());
}

std::unique_ptr<fys::arena::PartyTeam>
DBConnector::retrievePartyTeam(const std::string& user)
{
	auto team = std::make_unique<fys::arena::PartyTeam>(user);

	// Temporary hard coded party team
	auto tm1 = std::make_shared<fys::arena::TeamMember>(user, "Elvo");
	auto tm2 = std::make_shared<fys::arena::TeamMember>(user, "Mirael");
	auto tm3 = std::make_shared<fys::arena::TeamMember>(user, "Fyston");
	auto tm4 = std::make_shared<fys::arena::TeamMember>(user, "Simon");

	fys::arena::FightingPitLayout::setAllyMoveInitiatePosition(*tm1, fys::arena::HexagonSide::Orientation::B_S);
	auto& s1 = tm1->accessStatus();
	s1.life.total = 100;
	s1.life.current = 100;
	s1.magicPoint.total = 20;
	s1.magicPoint.current = 20;
	s1.initialSpeed = 3;
	fys::arena::FightingPitLayout::setAllyMoveInitiatePosition(*tm2, fys::arena::HexagonSide::Orientation::B_S);
	auto& s2 = tm2->accessStatus();
	s2.life.total = 200;
	s2.life.current = 200;
	s2.magicPoint.total = 0;
	s2.magicPoint.current = 0;
	s2.initialSpeed = 5;
	fys::arena::FightingPitLayout::setAllyMoveInitiatePosition(*tm3, fys::arena::HexagonSide::Orientation::B_S);
	auto& s3 = tm3->accessStatus();
	s3.life.total = 550;
	s3.life.current = 550;
	s3.magicPoint.total = 10;
	s3.magicPoint.current = 10;
	s3.initialSpeed = 10;
	fys::arena::FightingPitLayout::setAllyMoveInitiatePosition(*tm4, fys::arena::HexagonSide::Orientation::B_S);
	auto& s4 = tm4->accessStatus();
	s4.life.total = 140;
	s4.life.current = 140;
	s4.magicPoint.total = 10;
	s4.magicPoint.current = 10;
	s4.initialSpeed = 20;

	tm1->addDoableAction("arena:actions:damage:slash.chai", 2);
	tm1->addDoableAction("arena:actions:misc:meditate.chai", 2);

	tm2->addDoableAction("arena:actions:damage:slash.chai", 2);
	tm2->addDoableAction("arena:actions:misc:meditate.chai", 2);

	tm3->addDoableAction("arena:actions:damage:slash.chai", 2);
	tm3->addDoableAction("arena:actions:misc:meditate.chai", 2);

	tm4->addDoableAction("arena:actions:damage:slash.chai", 2);
	tm4->addDoableAction("arena:actions:misc:meditate.chai", 2);

	team->addTeamMember(std::move(tm1));
	team->addTeamMember(std::move(tm2));
	team->addTeamMember(std::move(tm3));
	team->addTeamMember(std::move(tm4));

//        mariadb::connection_ref con = mariadb::connection::create(_refDb);
//        mariadb::statement_ref stmt = con->create_statement("SELECT * FROM team_character WHERE id_user=?");
//        stmt->set_string(0, user);
//        auto result = stmt->query();
//        res.reserve(result->row_count());
//        while (result->next()) {
//            fys::arena::TeamMember tm;
//            tm.accessStatus().life.current = result->get_unsigned32("life");
//            tm.accessStatus().life.total = result->get_unsigned32("life_max");
//            tm.setName(result->get_string("name"));
//            res.emplace_back(std::move(tm));
//        }
	return team;
}

std::vector<std::string>
DBConnector::retrieveAttacksKeys(const std::string& userName)
{
	auto res = std::vector<std::string>();
	return res;
}

}