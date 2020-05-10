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

#include <fightingPit/data/CommonTypes.hh>
#include <fightingPit/contender/PitContenders.hh>
#include <fightingPit/contender/FightingContender.hh>
#include <fightingPit/team/TeamMember.hh>
#include <fightingPit/team/AllyPartyTeams.hh>
#include <HistoryManager.hh>

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
		return actionName == fys::arena::data::getActionNameFromKey(action.first);
	};
}

}

namespace fys::arena {

bool
TeamMember::executeAction(
		AllyPartyTeams& apt,
		PitContenders& pc,
		std::unique_ptr<chaiscript::ChaiScript>& chaiPtr)
{
	auto pa = _pendingActions.pop();

	if (!pa) {
		// No action to execute in the pipeline for the team member
		return false;
	}
	if (pa->idAction >= _actionsDoable.size()) {
		SPDLOG_ERROR("TeamMember {}::{} id {} tried to execute a non existing action of id {}",
				_userName, _name, _id, pa->idAction);
		return false;
	}
	const std::string actionKey = _actionsDoable.at(pa->idAction).first;
	const std::string allyAction = chai::util::getAccessAllyAction(_userName, _name,
			data::getActionNameFromKey(actionKey));
	const std::string funActionStr = fmt::format(R"(fun(target){{ return {}.execute(target);}})", allyAction);
	data::Targeting targetType;

	try {
		targetType = chaiPtr->eval<data::Targeting>(allyAction + ".requireTarget();");
	}
	catch (const std::exception& e) {
		SPDLOG_ERROR("action retrieved by {} doesn't have requireTarget method {}", allyAction, e.what());
		return false;
	}

	HistoryManager::addHistoric(apt.getFightingPitId(), HistoryAction{_id, _name, actionKey, pa->target});

	try {
		// If a specific target is required, otherwise self is used
		if (pa->target.has_value()) {
			std::visit(overloaded{
					[&apt, &targetType, &funActionStr, &chaiPtr](AllyTargetId target) {
						if (targetType == data::ALLY || targetType == data::ALLY_OR_ENNEMY) {
							const auto funcAction = chaiPtr->eval<std::function<int(TeamMember&)>>(funActionStr);
							funcAction(*apt.selectMemberById(target.v));
						}
						else {
							spdlog::error("Action of type {}, couldn't target an AllyTarget of id {}", targetType, target.v);
						}
					},

					[&pc, &targetType, &funActionStr, &chaiPtr](ContenderTargetId target) {
						if (targetType == data::ENNEMY || targetType == data::ALLY_OR_ENNEMY) {
							const auto funcAction = chaiPtr->eval<std::function<int(FightingContender&)>>(funActionStr);
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
			const auto funcAction = chaiPtr->eval<std::function<int(TeamMember&)>>(funActionStr);
			funcAction(*this);
		}
	}
	catch (const chaiscript::exception::eval_error& ee) {
		SPDLOG_ERROR("Error caught on script execution while executing {} with target required {}. Team owned by "
					 "{} TeamMember {} --> {}", pa->idAction, static_cast<bool>(pa->target), _userName, _name, ee.what());
		return false;
	}
	return true;
}

void
TeamMember::addPendingAction(const std::string& actionName, std::optional<TargetType> target)
{
	if (_status.life.isDead()) {
		SPDLOG_WARN("Player {}::{} tried to add an action while dead", _userName, _name);
		return;
	}
	auto it = std::find_if(_actionsDoable.begin(), _actionsDoable.end(), actionMatchKey(actionName));
	if (it == _actionsDoable.end()) {
		SPDLOG_WARN("Player {}::{} tried unrecognized action called {}", _userName, _name, actionName);
		return;
	}
	_pendingActions.push(PendingAction{static_cast<uint>(std::distance(_actionsDoable.begin(), it)), std::move(target)});
}

}
