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


#ifndef FYS_ONLINE_INVENTORY_SERVER_CONTEXT_HH
#define FYS_ONLINE_INVENTORY_SERVER_CONTEXT_HH

#include <nlohmann/json.hpp>
#include <service_context_base.hh>

namespace fys::inv {

class inventory_server_context : public common::service_context_base {

public:
	explicit inventory_server_context(int argc, char** argv);

	[[nodiscard]] const std::string&
	getConnectionStringCacheDB() const { return _connectionStringCacheDB; }

	[[nodiscard]] const std::string&
	getServerCode() const { return _serverCode; }

	[[nodiscard]] std::string
	getPlayerConnectionString() const noexcept;

	[[nodiscard]] std::string
	toString() const;

private:
	void initInventoryContextWithJson(nlohmann::json& json);

private:
	std::string _connectionStringCacheDB;
	std::string _serverCode;

	uint _portPlayerConnection;

};

}

#endif //FYS_ONLINE_INVENTORY_SERVER_CONTEXT_HH
