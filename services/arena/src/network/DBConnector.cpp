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

#include <mariadb++/account.hpp>
#include <mariadb++/statement.hpp>
#include <ArenaServerContext.hh>
#include <network/DBConnector.hh>

namespace fys::network {

    DBConnector::DBConnector(const arena::ArenaServerContext &ctx) {
        _refDb = mariadb::account::create(ctx.getDbHost(), "Arena", "ArenaPWD", "fys", ctx.getDbPort());
    }

    std::vector<fys::arena::TeamMember> DBConnector::retrieveCharacters() {
        auto res = std::vector<fys::arena::TeamMember>();
        return res;
    }

    std::vector<std::string> DBConnector::retrieveAttacksKeys() {
        auto res = std::vector<std::string>();
        return res;
    }


}