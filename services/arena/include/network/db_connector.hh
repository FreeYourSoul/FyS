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


#ifndef FYS_SERVICE_DBCONNECTOR_HH
#define FYS_SERVICE_DBCONNECTOR_HH

#include <memory>

namespace mariadb {
class account;
using account_ref = std::shared_ptr<account>;
}

namespace fys::arena {
class arena_server_context;
class party_team;
}

namespace fys::network {

class db_connector {

public:
	explicit db_connector(const arena::arena_server_context& ctx);

	[[nodiscard]] std::unique_ptr<fys::arena::party_team>
	retrieve_party_team(const std::string& user);

private:
	[[nodiscard]] std::vector<std::string>
	retrieve_attacks_keys(const std::string& user_name);

private:
	mariadb::account_ref _ref_db;
};

}

#endif //FYS_SERVICE_DBCONNECTOR_HH