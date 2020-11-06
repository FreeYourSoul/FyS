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

#include <spdlog/spdlog.h>

#include <chaiscript/chaiscript.hpp>
#include <chaiscript/utility/utility.hpp>

#include <chai_register.hh>
#include <fightingPit/contender/contender_scripting.hh>

using namespace chaiscript;

namespace fys::arena {

void contender_scripting::register_contender_script() {
  const std::string create_var = fmt::format("global {}={}({},{});",
											 chai_contender_id(), _contender_name, _contender_id, _level);
  const std::string retrieve_doable_actions = fmt::format("retrieveDoableActions({}.actions.act);", chai_contender_id());

  // register instance in ChaiScript
  try {
	_chai.get().eval(create_var);
  } catch (const chaiscript::exception::eval_error &ee) {
	SPDLOG_ERROR("Error caught when instantiating variable (script : '{}')\n{}", create_var, ee.what());
  }

  // load doable actions
  try {
	_doable_actions = _chai.get().eval<std::vector<std::string>>(retrieve_doable_actions);
  } catch (const chaiscript::exception::eval_error &ee) {
	SPDLOG_ERROR("Error caught when retrieving doable actions (script : '{}')\n{}", retrieve_doable_actions, ee.what());
  }
}

void contender_scripting::load_contender_script_from_file(const std::string &file_path) {
  // load script content (containing the class)
  try {
	_chai.get().use(file_path);
  } catch (const chaiscript::exception::eval_error &ee) {
	SPDLOG_WARN("Error caught on scripting loading\n{}\n{}", ee.what(), ee.detail);
  }
  register_contender_script();
}

bool contender_scripting::setup_contender() {
  try {
	_chai.get().eval(chai_contender_id() + ".setupContender();");
  } catch (const chaiscript::exception::eval_error &ee) {
	SPDLOG_ERROR("setupContender failed for {} :\n{}", chai_contender_id(), ee.what());
	return false;
  }
  return true;
}

void contender_scripting::execute_action() {
  std::string action = fmt::format("fun(contenderId){{ return {}.runScriptedAction(contenderId);}}", chai_contender_id());
  try {
	auto funcAction = _chai.get().eval<std::function<int(unsigned int)>>(action);
	if (funcAction(_contender_id)) {
	  SPDLOG_DEBUG("Contender {}_{} executed action", _contender_name, _contender_id);
	}
  } catch (const chaiscript::exception::eval_error &ee) {
	SPDLOG_ERROR("Error caught on script execution while executing action ('{}') of contender.\n{}",
				 action, ee.what());
  }
}
}// namespace fys::arena
