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

#include <service_context_base.hh>
#include <nlohmann/json.hpp>
#include <engine/collision_map.hh>

namespace fys::ws {

class world_server_context : fys::common::service_context_base {

public:
	explicit world_server_context(int ac, const char* const* av);

	[[nodiscard]] std::string
	to_string() const noexcept;

	[[nodiscard]] std::string
	get_dispatcher_sub_connection_str() const noexcept;

	[[nodiscard]] std::string
	get_dispatcher_connection_str() const noexcept;

	[[nodiscard]] std::string
	getPlayerConnectionString() const noexcept;

	[[nodiscard]] const std::string&
	getPathToLocalStorage() const noexcept { return _pathToLocalStorage; }

	[[nodiscard]] const std::string&
	getServerCode() const noexcept { return _serverCode; }

	[[nodiscard]] const std::string&
	getTMXMapPath() const noexcept { return _tmxMapPath; }

	[[nodiscard]] const boundary&
	getServerXBoundaries() const noexcept { return _serverXBoundaries; }

	[[nodiscard]] const boundary&
	getServerYBoundaries() const noexcept { return _serverYBoundaries; }

	[[nodiscard]] const std::vector<proximity_server>&
	getServerProximity() const noexcept { return _serverProximity; }

	[[nodiscard]] const std::string&
	getPathToLuaInitEngine() const noexcept { return _pathToLuaInitEngine; }

	[[nodiscard]] const std::string&
	getSpawningConfigPath() const noexcept { return _spawningConfigPath; }

	[[nodiscard]] const std::string&
	getPathLuaBase() const noexcept { return _pathLuaBase; }

private:
	void initWsContextWithJson(nlohmann::json& json);

private:
	std::string _serverCode;
	std::string _tmxMapPath;
	std::string _pathToLocalStorage;
	std::string _pathLuaBase;
	std::string _pathToLuaInitEngine;
	std::string _spawningConfigPath;

	uint _portPlayerConnection;
	boundary _serverXBoundaries;
	boundary _serverYBoundaries;
	std::vector<proximity_server> _serverProximity;

};

}

#endif //FYS_WORLDSERVERCONTEXT_HH
