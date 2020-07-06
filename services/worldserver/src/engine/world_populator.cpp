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

#include <fstream>

#include <CmlKey.hh>
#include <world_server_context.hh>
#include <engine/script_engine.hh>
#include <engine/engine.hh>
#include <engine/world_populator.hh>

namespace {
void
assertEngineError(bool isError, const std::string& errorMsg)
{
	if (isError) {
		std::string err = std::string("Miss constructed WorldServerEngine : ").append(errorMsg);
		SPDLOG_ERROR(err);
		throw std::runtime_error(err);
	}
}

[[nodiscard]] inline std::string
getSPNamespaceFromKey(const std::string& key)
{
	auto startSeparator = key.find_last_of(':');
	auto endSeparator = key.find_last_of('.');

	if (startSeparator != std::string::npos && endSeparator != std::string::npos) {
		// if a ':' and a '.' are found
		return key.substr(startSeparator + 1, endSeparator - startSeparator - 1);
	}
	else if (startSeparator == std::string::npos && endSeparator != std::string::npos) {
		// if a ':' is not found but not a '.' is found
		return key.substr(0, endSeparator);
	}
	else if (startSeparator != std::string::npos && endSeparator == std::string::npos) {
		// if a ':' is found but not a '.' is not found
		return key.substr(startSeparator + 1);
	}
	return key;
}

[[nodiscard]] inline std::string
getPathFromKey(std::string base, std::string key)
{
	std::replace(key.begin(), key.end(), ':', '/');
	return base.append("/").append(std::move(key));
}

}

namespace fys::ws {

std::shared_ptr<engine>
world_populator::buildWorldServerEngine()
{
	SPDLOG_INFO("[INIT] Start building ServerEngine...");

	::assertEngineError(_connectionString.empty(), "Connection string is empty");
	::assertEngineError(static_cast<bool>(_scriptEngine) == false, "Script engine not initialized");
	::assertEngineError(static_cast<bool>(_map) == false, "Map is not initialized");

	SPDLOG_INFO("[INIT] ServerEngine setup is correct...");
	auto ret = std::make_shared<engine>
			(_connectionString, std::move(*_map.get()), std::move(_scriptEngine), _intervalMovement);
	SPDLOG_INFO("[INIT] ServerEngine building is complete");
	return ret;
}

world_populator&
world_populator::populateScriptEngine(const world_server_context& ctx)
{
	_scriptEngine = std::make_unique<script_engine>();
	registerCommonLuaEngine(ctx.getPathToLuaInitEngine());
	generateSpawningPoints(ctx.getSpawningConfigPath(), ctx.getPathLuaBase());
	return *this;
}

world_populator&
world_populator::populateMap(const world_server_context& ctx)
{
	// todo : define map and implement it
	return *this;
}

void
world_populator::generateSpawningPoints(const std::string& spawningPointConfigPath, const std::string& basePath)
{
	std::ifstream i(spawningPointConfigPath);
	nlohmann::json jsonConfig;
	i >> jsonConfig;

	auto wsJson = jsonConfig["spawning_points"];
	unsigned index = 0;

	_scriptEngine->_spawningPoints.resize(wsJson.size());
	for (auto &[key, value] : wsJson.items()) {
		const std::string keyCml = value["key_zone"].get<std::string>();
		const std::string spNamespace = getSPNamespaceFromKey(keyCml);

		_scriptEngine->_spawningPoints[index].spawning_interval = std::chrono::seconds(value["spawning_interval"].get<uint>());
		_scriptEngine->_spawningPoints[index].id_spawning_point = spNamespace;
		_scriptEngine->_spawningPoints[index].display_key = value["display_key"].get<std::string>();

		try {
			_scriptEngine->_lua.safe_script_file(getPathFromKey(basePath, keyCml));
			auto maxSpawn = _scriptEngine->_lua[spNamespace]["numbers"];
			auto visibilityDistance = _scriptEngine->_lua[spNamespace]["visibility_distance"];
			auto centerPos_y = _scriptEngine->_lua[spNamespace]["center_point"]["y"];
			auto centerPos_x = _scriptEngine->_lua[spNamespace]["center_point"]["x"];

			if (!visibilityDistance.valid()) {
				throw std::runtime_error("'visibility_distance' is not properly set'");
			}
			if (!maxSpawn.valid()) {
				throw std::runtime_error("'maxSpawn' is not properly set");
			}
			if (!centerPos_x.valid(), !centerPos_y.valid()) {
				throw std::runtime_error("'center_pos' is not properly set");
			}
			_scriptEngine->_spawningPoints[index].center_spawning_point = pos{
					static_cast<double>(centerPos_x),
					static_cast<double>(centerPos_y)
			};
			_scriptEngine->_spawningPoints[index].max_spawned = static_cast<uint>(maxSpawn);
			_scriptEngine->_spawningPoints[index].distance_notification = static_cast<uint>(visibilityDistance);

		}
		catch (const std::exception& e) {
			SPDLOG_ERROR("[INIT] An error occurred while instantiating SpawningPoints '{}' : {}",
					_scriptEngine->_spawningPoints[index].display_key, e.what());
		}
		++index;
	}
}

void
world_populator::registerCommonLuaEngine(const std::string& pathToLuaInitFile)
{
	_scriptEngine->_lua.open_libraries(sol::lib::base, sol::lib::package);

	try {
		_scriptEngine->_lua.safe_script_file(pathToLuaInitFile);
	}
	catch (const std::exception& e) {
		SPDLOG_ERROR("Error while initiating LUA engine : {} ", e.what());
	}

	auto position = _scriptEngine->_lua.new_usertype<pos>("Pos");
	position["x"] = &pos::x;
	position["y"] = &pos::y;

	auto characterInfo = _scriptEngine->_lua.new_usertype<character_info>("CharacterInfo");
	characterInfo["pos"] = &character_info::position;
	characterInfo["velocity"] = &character_info::velocity;
	characterInfo["angle"] = &character_info::angle;

	_scriptEngine->_lua["retrieveAngle"] = [](double x, double y, double destinationX, double destinationY) {
		return std::atan((y - destinationY) / (x - destinationX));
	};
}

const std::vector<spawning_point>&
world_populator::getSpawningPoints() const
{
	return _scriptEngine->_spawningPoints;
}

world_populator&
world_populator::setConnectionString(std::string connectionString)
{
	_connectionString = std::move(connectionString);
	return *this;
}

world_populator&
world_populator::setIntervalMovement(std::chrono::system_clock::duration intervalMovement)
{
	_intervalMovement = intervalMovement;
	return *this;
}

}
