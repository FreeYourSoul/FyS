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

#include <fmt/ostream.h>
#include <spdlog/spdlog.h>

#include <chaiscript/chaiscript.hpp>
#include <util/ChaiUtility.hh>

#include <fightingPit/data/common_types.hh>
#include <fightingPit/contender/pit_contenders.hh>
#include <fightingPit/contender/fighting_contender.hh>
#include <fightingPit/team/team_member.hh>
#include <fightingPit/team/ally_party_teams.hh>
#include <history_manager.hh>

// overloaded trick
template<class... Ts>
struct overloaded : Ts ... {
	using Ts::operator()...;
};
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace {

[[nodiscard]] auto
actionMatchKey(const std::string& actionName)
{
	return [&actionName](const auto& action) {
		return actionName == fys::arena::data::get_action_name_from_key(action.first);
	};
}

}

namespace fys::arena {

bool
team_member::execute_action(
		ally_party_teams& apt,
		pit_contenders& pc,
		std::unique_ptr<chaiscript::ChaiScript>& chai_ptr)
{
	auto pa = _pending_actions.pop();

	if (!pa) {
		// No action to execute in the pipeline for the team member
		return false;
	}
	if (pa->id_action >= _actions_doable.size()) {
		SPDLOG_ERROR("TeamMember {}::{} id {} tried to execute a non existing action of id {}",
				_user_name, _name, _id, pa->id_action);
		return false;
	}
	const std::string actionKey = _actions_doable.at(pa->id_action).first;
	const std::string allyAction = chai::util::getAccessAllyAction(_user_name, _name,
			data::get_action_name_from_key(actionKey));
	const std::string funActionStr = fmt::format(R"(fun(target){{ return {}.execute(target);}})", allyAction);
	data::targeting targetType;

	try {
		targetType = chai_ptr->eval<data::targeting>(allyAction + ".requireTarget();");
	}
	catch (const std::exception& e) {
		SPDLOG_ERROR("action retrieved by {} doesn't have requireTarget method {}", allyAction, e.what());
		return false;
	}

	history_manager::addHistoric(apt.get_fighting_pit_id(), history_action{_id, _name, actionKey, pa->target});

	try {
		// If a specific target is required, otherwise self is used
		if (pa->target.has_value()) {
			std::visit(overloaded{
					[&apt, &targetType, &funActionStr, &chai_ptr](ally_target_id target) {
						if (targetType == data::ALLY || targetType == data::ALLY_OR_ENNEMY) {
							const auto funcAction = chai_ptr->eval<std::function<int(team_member&)>>(funActionStr);
							funcAction(*apt.select_member_by_id(target.v));
						}
						else {
							spdlog::error("Action of type {}, couldn't target an AllyTarget of id {}", targetType, target.v);
						}
					},

					[&pc, &targetType, &funActionStr, &chai_ptr](contender_target_id target) {
						if (targetType == data::ENNEMY || targetType == data::ALLY_OR_ENNEMY) {
							const auto funcAction = chai_ptr->eval<std::function<int(fighting_contender&)>>(funActionStr);
							funcAction(*pc.getFightingContender(target.v));
						}
						else {
							spdlog::error("Action of type {}, couldn't target a ContenderTarget of id {}", targetType, target.v);
						}
					},

					[]([[maybe_unused]] auto o) {
						spdlog::error("NOT IMPLEMENTED YET");
					}
			}, *pa->target);
		}
		else {
			const auto funcAction = chai_ptr->eval<std::function<int(team_member&)>>(funActionStr);
			funcAction(*this);
		}
	}
	catch (const chaiscript::exception::eval_error& ee) {
		SPDLOG_ERROR("Error caught on script execution while executing {} with target required {}. Team owned by "
					 "{} TeamMember {} --> {}", pa->id_action, static_cast<bool>(pa->target), _user_name, _name, ee.what());
		return false;
	}
	return true;
}

void
team_member::add_pending_action(const std::string& action_name, std::optional<TargetType> target)
{
	if (_status.life_pt.is_dead()) {
		SPDLOG_WARN("Player {}::{} tried to add an action while dead", _user_name, _name);
		return;
	}
	auto it = std::find_if(_actions_doable.begin(), _actions_doable.end(), actionMatchKey(action_name));
	if (it == _actions_doable.end()) {
		SPDLOG_WARN("Player {}::{} tried unrecognized action called {}", _user_name, _name, action_name);
		return;
	}
	_pending_actions.push(pending_action{static_cast<uint>(std::distance(_actions_doable.begin(), it)), std::move(target)});
}

}
