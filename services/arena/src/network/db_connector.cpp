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
#include <arena_server_context.hh>
#include <fightingPit/team/party_team.hh>
#include <fightingPit/team/team_member.hh>
#include <network/db_connector.hh>

namespace fys::network {

db_connector::db_connector(const arena::arena_server_context& ctx) {
  //        _refDb = mariadb::account::create(ctx.getDbHost(), "Arena", "ArenaPWD", "fys", ctx.getDbPort());
}

std::unique_ptr<fys::arena::party_team>
db_connector::retrieve_party_team(const std::string& user) {
  auto team = std::make_unique<fys::arena::party_team>(user);

  // Temporary hard coded party team
  auto tm1 = std::make_shared<fys::arena::team_member>(user, "Elvo");
  auto tm2 = std::make_shared<fys::arena::team_member>(user, "Mirael");
  auto tm3 = std::make_shared<fys::arena::team_member>(user, "Fyston");
  auto tm4 = std::make_shared<fys::arena::team_member>(user, "Simon");

  fys::arena::fighting_pit_layout::set_ally_move_initiate_position(*tm1, fys::arena::hexagon_side::orientation::B_S);
  auto& s1 = tm1->access_status();
  s1.life_pt.total = 100;
  s1.life_pt.current = 100;
  s1.magic_pt.total = 20;
  s1.magic_pt.current = 20;
  s1.initial_speed = 3;
  fys::arena::fighting_pit_layout::set_ally_move_initiate_position(*tm2, fys::arena::hexagon_side::orientation::B_S);
  auto& s2 = tm2->access_status();
  s2.life_pt.total = 200;
  s2.life_pt.current = 200;
  s2.magic_pt.total = 0;
  s2.magic_pt.current = 0;
  s2.initial_speed = 5;
  fys::arena::fighting_pit_layout::set_ally_move_initiate_position(*tm3, fys::arena::hexagon_side::orientation::B_S);
  auto& s3 = tm3->access_status();
  s3.life_pt.total = 550;
  s3.life_pt.current = 550;
  s3.magic_pt.total = 10;
  s3.magic_pt.current = 10;
  s3.initial_speed = 10;
  fys::arena::fighting_pit_layout::set_ally_move_initiate_position(*tm4, fys::arena::hexagon_side::orientation::B_S);
  auto& s4 = tm4->access_status();
  s4.life_pt.total = 140;
  s4.life_pt.current = 140;
  s4.magic_pt.total = 10;
  s4.magic_pt.current = 10;
  s4.initial_speed = 20;

  tm1->add_doable_action("arena:actions:damage:slash.chai", 2);
  tm1->add_doable_action("arena:actions:misc:meditate.chai", 2);

  tm2->add_doable_action("arena:actions:damage:slash.chai", 2);
  tm2->add_doable_action("arena:actions:misc:meditate.chai", 2);

  tm3->add_doable_action("arena:actions:damage:slash.chai", 2);
  tm3->add_doable_action("arena:actions:misc:meditate.chai", 2);

  tm4->add_doable_action("arena:actions:damage:slash.chai", 2);
  tm4->add_doable_action("arena:actions:misc:meditate.chai", 2);

  team->add_team_member(std::move(tm1));
  team->add_team_member(std::move(tm2));
  team->add_team_member(std::move(tm3));
  team->add_team_member(std::move(tm4));

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
db_connector::retrieve_attacks_keys(const std::string& user_name) {
  auto res = std::vector<std::string>();
  return res;
}

}// namespace fys::network