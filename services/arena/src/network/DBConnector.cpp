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
#include <fightingPit/team/TeamMember.hh>

namespace fys::network {

    DBConnector::DBConnector(const arena::ArenaServerContext &ctx) {
//        _refDb = mariadb::account::create(ctx.getDbHost(), "Arena", "ArenaPWD", "fys", ctx.getDbPort());
    }

    std::vector<fys::arena::TeamMember> DBConnector::retrieveTeamCharacters(const std::string &user) {
//        auto res = std::vector<fys::arena::TeamMember>();
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
        return res;
    }

    std::vector<std::string> DBConnector::retrieveAttacksKeys() {
        auto res = std::vector<std::string>();
        return res;
    }


}