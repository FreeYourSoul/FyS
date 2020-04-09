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

#ifndef FYS_ARENA_CHAIREGISTER_HH
#define FYS_ARENA_CHAIREGISTER_HH

#include <memory>

// forward declarations
namespace chaiscript {
class ChaiScript;
class Module;
using ModulePtr = std::shared_ptr<chaiscript::Module>;
}
namespace fys {
namespace cache {
class Cml;
}
namespace arena {
class ConnectionHandler;
class PitContenders;
class AllyPartyTeams;
class PartyTeam;
}
} // !forward declarations

namespace fys::arena {

class ChaiRegister {
public:
	[[nodiscard]] static std::unique_ptr<chaiscript::ChaiScript>
	createChaiInstance(PitContenders& pc, AllyPartyTeams& apt);

	static void
	registerUtility(chaiscript::ChaiScript& chai, PitContenders& pc, AllyPartyTeams& apt);

	static bool
	loadAndRegisterActionPartyTeam(chaiscript::ChaiScript& chai, cache::Cml& cache, const fys::arena::PartyTeam& pt);

	static void
	loadActionScripts(chaiscript::ChaiScript& chai, cache::Cml& cache, const std::vector<std::string> &scriptsKeys);

	static void
	registerBaseActions(chaiscript::ChaiScript& chai, cache::Cml& cml);

private:
	static void
	registerChai(chaiscript::ChaiScript& chai, fys::arena::PitContenders& pc, fys::arena::AllyPartyTeams& apt);

	static void
	registerCommon(chaiscript::ModulePtr m);

	static void
	registerFightingPitContender(chaiscript::ChaiScript& chai, chaiscript::ModulePtr m);

	static void
	registerTeamAllies(chaiscript::ChaiScript& chai, chaiscript::ModulePtr m);

};

} // !fys::arena

#endif // !FYS_ARENA_CHAIREGISTER_HH
