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


#include <engine/WorldPopulator.hh>

namespace fys::ws {

WorldServerEngine
WorldPopulator::buildWorldServerEngine(const WorldServerContext& ctx) const
{
	return ws::WorldServerEngine(ctx);
}

void
WorldPopulator::generateSpawningPoints(const std::string& spawningPointConfigPath)
{
}

void
WorldPopulator::registerCommonLuaEngine()
{
	SPDLOG_INFO("Register LUA utilities");

	_scriptEngine._lua.open_libraries(sol::lib::base, sol::lib::package);

	try {
		_lua.safe_script(R"(
			function getCharacterInfo(spawningPoint, luaId)
				if luaId >= spawningPoint.numbers then return nil end
			end

			function spawn(spawningPoint)
				for id, spawn in pairs(spawningPoint.spawned) do
					if spawn.isAlive == false then
						spawningPoint.spawned[id].isAlive = true
						return id
					end
				end
				return nil
			end

			function print_test()
				print("This is a print test")
			end

			function execMovement(spawningPoint)


				-- Increment the current step (maybe # can be used instead of numberSteps)
				spawningPoint.current = current + 1
				if spawningPoint.current == spawningPoint.numberSteps then
					spawningPoint.current = 0
				end
			end
			)");
	}
	catch (const std::exception& e) {
		SPDLOG_ERROR("Error while registering basic content {} ", e.what());
	}

	auto position = _scriptEngine._lua.new_usertype<Pos>("Pos");
	position["x"] = &Pos::x;
	position["y"] = &Pos::y;

	auto characterInfo = _scriptEngine._lua.new_usertype<CharacterInfo>("CharacterInfo");
	characterInfo["pos"] = &CharacterInfo::pos;
	characterInfo["velocity"] = &CharacterInfo::velocity;
	characterInfo["angle"] = &CharacterInfo::angle;

	_lua["retrieveAngle"] = [](double x, double y, double destinationX, double destinationY) {
		return std::atan((y - destinationY) / (x - destinationX));
	};

}

}
