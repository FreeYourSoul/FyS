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


#ifndef FYS_ONLINE_WORLDPOPULATOR_HH
#define FYS_ONLINE_WORLDPOPULATOR_HH

namespace fys::ws {
class WorldServerContext;
class CollisionMap;
class ScriptEngine;
class WorldServerEngine;
struct SpawningPoint;
}

namespace fys::ws {

static constexpr std::chrono::milliseconds TIMING_MOVE_INTERVAL(16);


/**
 * WorldPopulator is used in order to get the data required to populate the section of the map handled by
 * the World Server Service. It request a database in order to:
 * - download NPC scripts, and trigger, to be used by Cml (Cache Manager Library) afterwards
 * - retrieve the NPC position (with their associated script id)
 * - retrieve the Monster spawn position (with their associated script id)
 * - get the trigger points (with their associated script id) of the map and set them in the map
 */
class WorldPopulator {

public:
	[[nodiscard]] std::unique_ptr<WorldServerEngine>
	buildWorldServerEngine();

	// Builder methods
	WorldPopulator& populateMap(const WorldServerContext& ctx);
	WorldPopulator& populateScriptEngine(const WorldServerContext& ctx);
	WorldPopulator& setConnectionString(std::string connectionString);
	WorldPopulator& setIntervalMovement(std::chrono::system_clock::duration intervalMovement);

	// Getters for testing
	const std::vector<SpawningPoint> &getSpawningPoints() const;

private:
	void registerCommonLuaEngine(const std::string& pathToLuaInitFile);
	void generateSpawningPoints(const std::string& spawningPointConfigPath);

private:
	std::string _connectionString;
	std::chrono::system_clock::duration _intervalMovement = TIMING_MOVE_INTERVAL;
	std::unique_ptr<CollisionMap> _map;
	std::unique_ptr<ScriptEngine> _scriptEngine;

};

}

#endif //FYS_ONLINE_WORLDPOPULATOR_HH
