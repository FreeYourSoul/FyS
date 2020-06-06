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


#ifndef FYS_WORLDSERVERCONTEXT_HH
#define FYS_WORLDSERVERCONTEXT_HH

#include <ServiceContextBase.hh>
#include <nlohmann/json.hpp>
#include <engine/CollisionMap.hh>

namespace fys::ws {

class WorldServerContext : fys::common::ServiceContextBase {

public:
	WorldServerContext(int ac, const char* const* av);

	[[nodiscard]] std::string
	toString() const noexcept;

	[[nodiscard]] std::string
	getDispatcherSubConnectionString() const noexcept;

	[[nodiscard]] std::string
	getDispatcherConnectionString() const noexcept;

	[[nodiscard]] std::string
	getPlayerConnectionString() const noexcept;

	[[nodiscard]] const std::string&
	getPathToLocalStorage() const noexcept;

	[[nodiscard]] const std::string&
	getServerCode() const noexcept;

	[[nodiscard]] const std::string&
	getTMXMapPath() const noexcept;

	[[nodiscard]] const Boundary&
	getServerXBoundaries() const noexcept;

	[[nodiscard]] const Boundary&
	getServerYBoundaries() const noexcept;

	[[nodiscard]] const std::vector<ProximityServer>&
	getServerProximity() const noexcept;

	[[nodiscard]] const std::string &
	getPathToLuaInitEngine() const noexcept;

private:
	void initWsContextWithJson(nlohmann::json& json);

private:
	std::string _serverCode;
	std::string _tmxMapPath;
	std::string _pathToLocalStorage;
	std::string _pathToLuaInitEngine;

	uint _portPlayerConnection;
	Boundary _serverXBoundaries;
	Boundary _serverYBoundaries;
	std::vector<ProximityServer> _serverProximity;

};

}

#endif //FYS_WORLDSERVERCONTEXT_HH
