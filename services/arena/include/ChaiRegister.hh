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

namespace data {
class Status;
}

}
}// !forward declarations

namespace fys::arena {

class ChaiRegister {
public:
	[[nodiscard]] static std::unique_ptr<chaiscript::ChaiScript>
	createChaiInstance(PitContenders& pc, AllyPartyTeams& apt);

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

	/**
	 * Get given action scripts from Cml and load it into the ChaiScript engine.
	 * Only load the script in the chai engine and do not instantiate any element in it.
	 * Then load eventual alteration scripts.
	 *
	 * @param chai engine to load into
	 * @param cache Cml instance to retrieve from
	 * @param keys keys to find the scripts
	 */
	static void
	loadActionScripts(chaiscript::ChaiScript& chai, cache::Cml& cache, const std::vector<std::string>& keys);

	static void
	registerBaseActions(chaiscript::ChaiScript& chai, cache::Cml& cml);

private:
	/**
	 * Get a script (actions, alterations etc..) from Cml and load it into the ChaiScript engine.
	 * Only load the script in the chai engine and do not instantiate any element in it.
	 *
	 * @param chai engine to load into
	 * @param cache Cml instance to retrieve from
	 * @param keys keys to find the scripts
	 */
	static void
	loadScripts(chaiscript::ChaiScript& chai, cache::Cml& cache, const std::vector<std::string>& keys);

	/**
	 * Retrieve the alterations (if any) of an action and load it into the ChaiScript engine
	 * @param chai engine to load into
	 * @param cache Cml instance to retrieve from
	 * @param keys actions keys to find the action on which alternate could be
	 */
	static void
	loadActionsAlterationsScript(chaiscript::ChaiScript& chai, cache::Cml& cache, const std::vector<std::string>& keys);

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
