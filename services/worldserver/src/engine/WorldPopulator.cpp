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

#include <WorldServerContext.hh>
#include <engine/WorldServerEngine.hh>
#include <engine/WorldPopulator.hh>

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
}

namespace fys::ws {

std::unique_ptr<WorldServerEngine>
WorldPopulator::buildWorldServerEngine()
{
	SPDLOG_INFO("[INIT] Start building ServerEngine...");

	::assertEngineError(_connectionString.empty(), "Connection string is empty");
	::assertEngineError(static_cast<bool>(_scriptEngine) == false, "Script engine not initialized");
	::assertEngineError(static_cast<bool>(_map) == false, "Map is not initialized");

	SPDLOG_INFO("[INIT] ServerEngine setup is correct...");
	auto ret = std::make_unique<WorldServerEngine>
			(_connectionString, std::move(*_map.get()), std::move(_scriptEngine), _intervalMovement);
	SPDLOG_INFO("[INIT] ServerEngine building is complete");
	return ret;
}

WorldPopulator&
WorldPopulator::populateScriptEngine(const WorldServerContext& ctx)
{
	_scriptEngine = std::make_unique<ScriptEngine>();
	registerCommonLuaEngine(ctx.getPathToLuaInitEngine());
	return *this;
}

WorldPopulator&
WorldPopulator::populateMap(const WorldServerContext& ctx)
{
	return *this;
}

void
WorldPopulator::generateSpawningPoints(const std::string& spawningPointConfigPath)
{
}

void
WorldPopulator::registerCommonLuaEngine(const std::string& pathToLuaInitFile)
{
	_scriptEngine->_lua.open_libraries(sol::lib::base, sol::lib::package);

	try {
		_scriptEngine->_lua.safe_script_file(pathToLuaInitFile);
	}
	catch (const std::exception& e) {
		SPDLOG_ERROR("Error while initiating LUA engine : {} ", e.what());
	}

	auto position = _scriptEngine->_lua.new_usertype<Pos>("Pos");
	position["x"] = &Pos::x;
	position["y"] = &Pos::y;

	auto characterInfo = _scriptEngine->_lua.new_usertype<CharacterInfo>("CharacterInfo");
	characterInfo["pos"] = &CharacterInfo::pos;
	characterInfo["velocity"] = &CharacterInfo::velocity;
	characterInfo["angle"] = &CharacterInfo::angle;

	_scriptEngine->_lua["retrieveAngle"] = [](double x, double y, double destinationX, double destinationY) {
		return std::atan((y - destinationY) / (x - destinationX));
	};

}

const std::vector<SpawningPoint>&
WorldPopulator::getSpawningPoints() const
{
	return _scriptEngine->_spawningPoints;
}

WorldPopulator&
WorldPopulator::setConnectionString(std::string connectionString)
{
	_connectionString = std::move(connectionString);
	return *this;
}

WorldPopulator&
WorldPopulator::setIntervalMovement(std::chrono::system_clock::duration intervalMovement)
{
	_intervalMovement = intervalMovement;
	return *this;
}

}
