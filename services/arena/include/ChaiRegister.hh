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
#include <set>

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
class FightingPitLayout;

namespace data {
struct Status;
}

}
}// !forward declarations

namespace fys::arena {

class ChaiRegister {
public:
	[[nodiscard]] static std::unique_ptr<chaiscript::ChaiScript>
	createChaiInstance(PitContenders& pc, AllyPartyTeams& apt, FightingPitLayout& layout);

	static void
	registerUtility(chaiscript::ChaiScript& chai, PitContenders& pc, AllyPartyTeams& apt);

	/**
	 * Load all the actions/alterations scripts of a party team (a new incoming player) retrieved from Cml and generate the instance
	 * of actions into the ChaiScript engine for it to be manipulated in-game.
	 *
	 * @param chai engine to load and register into
	 * @param cache Cml instance to retrieve from
	 * @param pt partyTeam from where the actions has to be found
	 * @return
	 */
	static bool
	loadAndRegisterActionPartyTeam(chaiscript::ChaiScript& chai, cache::Cml& cache, PartyTeam& pt);

	static void
	loadContenderScript(chaiscript::ChaiScript& chai, cache::Cml& cml, const std::string& contenderKey);

	static void
	registerBaseActions(chaiscript::ChaiScript& chai, cache::Cml& cml);

private:
	/**
	 * Retrieve a script and its includes (if any) of a script and load it into the ChaiScript engine
	 * Go recursively through all the includes to include.
	 *
	 * @note Only load scripts, doesn't instantiate
	 *
	 * @param chai engine to load into
	 * @param cache Cml instance to retrieve from
	 * @param keys script's keys to find the includes from
	 */
	static void loadWithIncludes(chaiscript::ChaiScript& chai, cache::Cml& cache, const std::set<std::string>& keys);
	static void loadWithIncludes(chaiscript::ChaiScript& chai, cache::Cml& cache,
			const std::vector<std::string>& keys, std::set<std::string> incursion);

	/**
	 * Get a script from Cml and load it into the ChaiScript engine.
	 * Only do a plain load of the scripts in the chai engine and do not instantiate any element in it.
	 *
	 * @param chai engine to load into
	 * @param cache Cml instance to retrieve from
	 * @param keys keys to find the scripts
	 */
	static inline void
	loadScripts(chaiscript::ChaiScript& chai, cache::Cml& cache, const std::vector<std::string>& keys);

	static inline bool
	loadScript(chaiscript::ChaiScript& chai, cache::Cml& cache, const std::string& keys);

	static void
	registerChai(chaiscript::ChaiScript& chai, PitContenders& pc, AllyPartyTeams& apt, FightingPitLayout& layout);

	static void
	registerCommon(chaiscript::ModulePtr m, FightingPitLayout& layout);

	static void
	registerFightingPitContender(chaiscript::ChaiScript& chai, chaiscript::ModulePtr m);

	static void
	registerTeamAllies(chaiscript::ChaiScript& chai, chaiscript::ModulePtr m);

};

} // !fys::arena

#endif // !FYS_ARENA_CHAIREGISTER_HH
