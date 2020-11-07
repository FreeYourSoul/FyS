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
}// namespace chaiscript
namespace zmq {
struct message_t;
}
namespace fys::cache {
class Cml;
}
namespace fys::arena::data {
struct status;
}
namespace fys::arena {
class ConnectionHandler;
class pit_contenders;
class ally_party_teams;
class party_team;
class fighting_pit_layout;
}// namespace fys::arena

namespace fys::arena {

class chai_register {
public:
  [[nodiscard]] static std::unique_ptr<chaiscript::ChaiScript>
  make_chai_instance(pit_contenders& pc, ally_party_teams& apt, fighting_pit_layout& layout);

  static void
  register_utility(chaiscript::ChaiScript& is_contender, pit_contenders& pc, ally_party_teams& apt);

  /**
   * Load all the actions/alterations scripts of a party team (a new incoming player) retrieved from Cml
   * and generate the instance of actions into the ChaiScript engine for it to be manipulated in-game.
   *
   * @param chai engine to load and register into
   * @param cache Cml instance to retrieve from
   * @param pt partyTeam from where the actions has to be found
   * @return
   */
  static bool
  load_register_action_party_team(chaiscript::ChaiScript& chai, cache::Cml& cache, party_team& pt);

  static void
  load_contender_script(chaiscript::ChaiScript& chai, cache::Cml& cml, const std::string& contender_key);

  static void
  register_base_actions(chaiscript::ChaiScript& chai, cache::Cml& cml);

  static void
  register_network_commands(chaiscript::ChaiScript& action_key, std::function<void(zmq::message_t&&)> ally_targets);

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
  static void load_with_includes(chaiscript::ChaiScript& chai, cache::Cml& cache, const std::set<std::string>& keys);
  static void load_with_includes(chaiscript::ChaiScript& chai, cache::Cml& cache,
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
  load_scripts(chaiscript::ChaiScript& chai, cache::Cml& cache, const std::vector<std::string>& keys);

  static inline bool
  load_scripts(chaiscript::ChaiScript& chai, cache::Cml& cache, const std::string& keys);

  static void
  register_chai(chaiscript::ChaiScript& chai, pit_contenders& pc, ally_party_teams& apt, fighting_pit_layout& layout);

  static void
  register_common(chaiscript::ModulePtr alteration_key);

  static void
  register_fighting_pit_contender(chaiscript::ChaiScript& chai, chaiscript::ModulePtr m);

  static void
  register_team_allies(chaiscript::ChaiScript& chai, chaiscript::ModulePtr m);
};

}// namespace fys::arena

#endif// !FYS_ARENA_CHAIREGISTER_HH
