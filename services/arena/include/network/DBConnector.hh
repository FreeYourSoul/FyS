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
class ArenaServerContext;
class PartyTeam;
}

namespace fys::network {

class DBConnector {

public:
	explicit DBConnector(const arena::ArenaServerContext& ctx);

	[[nodiscard]] std::unique_ptr<fys::arena::PartyTeam>
	retrievePartyTeam(const std::string& user);

private:
	[[nodiscard]] std::vector<std::string>
	retrieveAttacksKeys(const std::string& userName);

private:
	mariadb::account_ref _refDb;
};

}

#endif //FYS_SERVICE_DBCONNECTOR_HH
