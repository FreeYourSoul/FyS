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
#include <fightingPit/data/CommonTypes.hh>
#include <fightingPit/contender/PitContenders.hh>
#include <fightingPit/contender/FightingContender.hh>
#include <fightingPit/team/TeamMember.hh>
#include <fightingPit/team/AllyPartyTeams.hh>

// overloaded trick
template<class... Ts>
struct overloaded : Ts ... {
	using Ts::operator()...;
};
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace {
std::string
getActionNameFromKey(const std::string& key)
{ // todo, put this code in a common zone + fix
	auto startSeparator = key.find_last_of(':');
	if (startSeparator == std::string::npos) return "";
	return key.substr(startSeparator + 1, key.find_last_of('.') - startSeparator - 1);
}
}

namespace fys::arena {

void
TeamMember::executeAction(
		AllyPartyTeams& apt,
		PitContenders& pc,
		std::unique_ptr<chaiscript::ChaiScript>& chaiPtr)
{
	auto pa = _pendingActions.pop();

	if (!pa) {
		SPDLOG_DEBUG("No action to execute in the pipeline for team member {}::{} id {}", _userName, _name, _id);
		return;
	}
	if (pa->idAction >= _actionsDoable.size()) {
		SPDLOG_ERROR("TeamMember {}::{} id {} tried to execute a non existing action of id {}",
				_userName, _name, _id, pa->idAction);
		return;
	}

	const std::string allyAction = fmt::format(
			R"(ally_actions["{}_{}"]["{}"])", _userName, _name, getActionNameFromKey(_actionsDoable.at(pa->idAction).first));
	const auto funcAction = chaiPtr->eval<std::function<int(data::Status&)>>(fmt::format(
			R"(fun(allyStatus){{ return {}.execute(allyStatus);}})", allyAction));
	data::Targeting targetType;

	try {
		targetType = chaiPtr->eval<data::Targeting>(allyAction + ".requireTarget();");
	}
	catch (const std::exception& e) {
		SPDLOG_ERROR("action retrieved by {} doesn't have requireTarget method {}", allyAction, e.what());
		return;
	}

	try {
		if (pa->target) {
			std::visit(overloaded{
					[&apt, &targetType, &funcAction](AllyTargetId target) {
						if (targetType == data::ALLY || targetType == data::ALLY_OR_ENNEMY) {
							funcAction(apt.selectMemberById(target.v)->accessStatus());
						}
						else {
							spdlog::error("Action of type {}, couldn't target an AllyTarget of id {}", targetType, target.v);
						}
					},

					[&pc, &targetType, &funcAction](ContenderTargetId target) {
						if (targetType == data::ENNEMY || targetType == data::ALLY_OR_ENNEMY) {
							funcAction(pc.getFightingContender(target.v)->accessStatus());
						}
						else {
							spdlog::error("Action of type {}, couldn't target a ContenderTarget of id {}", targetType, target.v);
						}
					},

					[]([[maybe_unused]] auto o) {
						spdlog::error("NOT IMPLEMENTED YET");
					},
			}, *pa->target);
		}
		else if (funcAction(_status)) {
			SPDLOG_DEBUG("Ally {}::{} id {} executed action {}", _userName, _name, _id, pa->idAction);
		}
	}
	catch (const chaiscript::exception::eval_error& ee) {
		SPDLOG_ERROR("Error caught on script execution while executing {} with target required {}. Team owned by {} TeamMember {} --> {}",
				pa->idAction, static_cast<bool>(pa->target), _userName, _name, ee.what());
	}
}

void
TeamMember::addPendingAction(const std::string& actionName, TargetType target)
{
	auto it = std::find_if(_actionsDoable.begin(), _actionsDoable.end(), [&actionName](const auto& action) {
		return actionName == getActionNameFromKey(action.first);
	});
	if (it == _actionsDoable.end()) {
		SPDLOG_WARN("Player {}::{} tried unrecognized action called {}", _userName, _name, actionName);
		return;
	}

	//TODO: add a check on whether the target is correct or not (contender or ally target)
	_pendingActions.push(PendingAction{static_cast<uint>(std::distance(_actionsDoable.begin(), it)), target});
}

}
