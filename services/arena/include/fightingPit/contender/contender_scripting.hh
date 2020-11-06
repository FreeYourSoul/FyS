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

#ifndef FYS_CONTENDERSCRIPTING_HH
#define FYS_CONTENDERSCRIPTING_HH

#include <memory>
#include <string>
#include <vector>

// forward declarations
namespace chaiscript {
class ChaiScript;
}
namespace fys::cache {
class Cml;
}
namespace fys::arena {
class pit_contenders;
class ally_party_teams;
}// namespace fys::arena

namespace fys::arena {

class contender_scripting {

public:
  explicit contender_scripting(chaiscript::ChaiScript &chai, uint level)
	  : _chai(chai), _level(level) {}

  void execute_action();

  /**
   * Method called after the contender has been added in the fighting pit, in order to initialize its stats
   */
  [[nodiscard]] bool
  setup_contender();

  //! fill _doable_actions and instantiate the contender in chai engine
  void register_contender_script();
  void load_contender_script_from_file(const std::string &file_path);
  void set_contender_name(std::string contender_name) { _contender_name = std::move(contender_name); }
  void set_contender_id(uint contender_id) { _contender_id = contender_id; }

  [[nodiscard]] const std::string &
  contender_name() const { return _contender_name; }

  [[nodiscard]] uint
  contender_id() const { return _contender_id; }

  [[nodiscard]] uint
  level() const { return _level; }

  [[nodiscard]] const std::vector<std::string> &
  doable_actions() const { return _doable_actions; }

private:
  [[nodiscard]] std::string
  chai_contender_id() const {
	return std::string("contender_").append(_contender_name).append(std::to_string(_contender_id));
  }

private:
  std::reference_wrapper<chaiscript::ChaiScript> _chai;
  std::string _contender_name;
  uint _level;
  uint _contender_id{};
  std::vector<std::string> _doable_actions;
};

using ContenderScriptingUPtr = std::unique_ptr<contender_scripting>;

}// namespace fys::arena

#endif// !FYS_CONTENDERSCRIPTING_HH
