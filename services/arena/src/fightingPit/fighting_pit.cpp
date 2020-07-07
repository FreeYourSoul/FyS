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
#include <algorithm>

#include <util/ChaiUtility.hh>
#include <chai_register.hh>

#include <algorithm/algorithm.hh>
#include <history_manager.hh>
#include <network/WorkerService.hh>

#include <fightingPit/contender/fighting_contender.hh>
#include <fightingPit/fighting_pit.hh>
#include <fightingPit/team/TeamMember.hh>
#include <flatbuffer_generator.hh>

namespace {
std::chrono::milliseconds
retrieveTimeInterludeFromLevelDegree(fys::arena::fighting_pit::level level)
{
	switch (level) {
		case fys::arena::fighting_pit::level::EASY:return fys::arena::interval::EASY;
		case fys::arena::fighting_pit::level::MEDIUM :return fys::arena::interval::MEDIUM;
		case fys::arena::fighting_pit::level::HARD :return fys::arena::interval::HARD;
		default:SPDLOG_ERROR("Incorrect level");
			return std::chrono::milliseconds{0};
	}
}
}

namespace fys::arena {

fighting_pit::fighting_pit(std::string creatorUserName, fys::arena::fighting_pit::level levelFightingPit)
		:
		_progress(progress::ON_HOLD),
		_level_fighting_pit(levelFightingPit),
		_time_interlude(retrieveTimeInterludeFromLevelDegree(_level_fighting_pit)),
		_layout_mapping(_contenders, _party_teams),
		_creator_user_name(std::move(creatorUserName)),
		_chai_ptr(chai_register::make_chai_instance(_contenders, _party_teams, _layout_mapping)),
		_rewards(std::make_unique<Rewards>()) { }

void
fighting_pit::continue_battle(const std::chrono::system_clock::time_point& now)
{
	for (auto& battle : _side_battles) {
		if (battle.empty()) { // if battle side is empty, ignore it
			continue;
		}
		auto currentParticipant = battle.getCurrentParticipantTurn(now, _time_interlude);
		bool turnExecuted = false;

		if (currentParticipant.isContender) {
			// non-playable character (enemy NPC)
			_contenders.executeContenderAction(currentParticipant);
			turnExecuted = true;
		}
		else {
			// character of a player
			turnExecuted = _party_teams.executeAllyAction(currentParticipant, _contenders, _chai_ptr);
		}
		battle.turnDone(turnExecuted);
		battle.eraseFinishedAlterationAndDeadCharactersFromTurnList();
	}
	// execute awaited movements
	_layout_mapping.executeMovements(_side_battles);

	_progress = update_progress_status();
}

fighting_pit::progress
fighting_pit::update_progress_status()
{
	if (_party_teams.allDead()) {
		return progress::CONTENDER_WIN;
	}
	else if (_contenders.allDead()) {
		history_manager::setToBeSaved(_arena_id, false);
		return progress::ALLY_WIN;
	}
	return progress::ON_GOING;
}

void
fighting_pit::forward_to_team_member(const std::string& user, PlayerAction action)
{
	auto member = _party_teams.getSpecificTeamMemberById(user, action.idMember);
	if (!member) {
		SPDLOG_ERROR("[fp:{}] : Player '{}' try to forward a message to not existing member member of id {}",
				_arena_id, user, action.idMember);
		return;
	}
	try {
		if (!chai::util::memberHasActionRegistered(*_chai_ptr, user, member->getName(), action.actionName)) {
			SPDLOG_WARN("[fp:{}] : Member {}.{}.{} tried to execute Action '{}' which isn't registered for member '{}'",
					_arena_id, user, member->getId(), member->getName(), action.actionName, member->getName());
			return;
		}
		auto[targetIsCorrect, target] = this->check_and_retrieve_target(user, member, action);
		if (targetIsCorrect) {
			SPDLOG_INFO("[fp:{}] : Member {}.{}.{} register a new action {}", _arena_id, user, member->getId(), member->getName(), action.actionName);
			member->addPendingAction(std::move(action.actionName), std::move(target));
		}
	}
	catch (const std::exception& e) {
		SPDLOG_ERROR("[fp:{}] : An error occurred when checking if an action was doable from user '{}':\n{}",
				_arena_id, user, e.what());
	}
}

void
fighting_pit::add_authenticated_user(std::string user_name, std::string user_token)
{
	_authenticated_players.push_back({std::move(user_name), std::move(user_token)});
}

bool
fighting_pit::is_player_participant(const std::string& name, const std::string& token) const
{
	return std::any_of(_authenticated_players.begin(), _authenticated_players.end(),
			[&name, &token](auto& authPlayer) { return authPlayer.name == name && authPlayer.token == token; });
}

void
fighting_pit::add_party_team_and_register_actions(std::unique_ptr<PartyTeam> pt, cache::Cml& cache)
{
	chai_register::load_register_action_party_team(*_chai_ptr, cache, *pt);
	_layout_mapping.addActivePartyTeam(*pt);
	_party_teams.addPartyTeam(std::move(pt));
	std::sort(_side_battles.begin(), _side_battles.end(),
			[this](auto& lhs, auto& rhs) {
				return _layout_mapping.activeCharactersOnSide(lhs.getSide()) < _layout_mapping.activeCharactersOnSide(rhs.getSide());
			});
}

// Initialization methods used by FightingPitAnnouncer

void
fighting_pit::initialize_side_battles()
{
	_side_battles.reserve(hexagon_side::SIDE_NUMBER);

	// Hexagon A creation
	_side_battles.emplace_back(_contenders, _party_teams, hexagon_side::Orientation::A_N);
	_side_battles.emplace_back(_contenders, _party_teams, hexagon_side::Orientation::A_NE);
	_side_battles.emplace_back(_contenders, _party_teams, hexagon_side::Orientation::A_SE);
	_side_battles.emplace_back(_contenders, _party_teams, hexagon_side::Orientation::A_S);
	_side_battles.emplace_back(_contenders, _party_teams, hexagon_side::Orientation::A_SW);
	_side_battles.emplace_back(_contenders, _party_teams, hexagon_side::Orientation::A_NW);

	// Hexagon B creation
	_side_battles.emplace_back(_contenders, _party_teams, hexagon_side::Orientation::B_N);
	_side_battles.emplace_back(_contenders, _party_teams, hexagon_side::Orientation::B_NE);
	_side_battles.emplace_back(_contenders, _party_teams, hexagon_side::Orientation::B_SE);
	_side_battles.emplace_back(_contenders, _party_teams, hexagon_side::Orientation::B_S);
	_side_battles.emplace_back(_contenders, _party_teams, hexagon_side::Orientation::B_SW);
	_side_battles.emplace_back(_contenders, _party_teams, hexagon_side::Orientation::B_NW);

	// Hexagon C creation
	_side_battles.emplace_back(_contenders, _party_teams, hexagon_side::Orientation::C_N);
	_side_battles.emplace_back(_contenders, _party_teams, hexagon_side::Orientation::C_NE);
	_side_battles.emplace_back(_contenders, _party_teams, hexagon_side::Orientation::C_SE);
	_side_battles.emplace_back(_contenders, _party_teams, hexagon_side::Orientation::C_S);
	_side_battles.emplace_back(_contenders, _party_teams, hexagon_side::Orientation::C_SW);
	_side_battles.emplace_back(_contenders, _party_teams, hexagon_side::Orientation::C_NW);

	initialize_priority_in_side_battles();
}

void
fighting_pit::initialize_priority_in_side_battles()
{
	for (auto& sb : _side_battles) {
		const auto& memberBySide = _party_teams.getMembersBySide(sb.getSide());
		const auto& contenderBySide = _contenders.getContenderOnSide(sb.getSide());

		for (const TeamMemberSPtr& member : memberBySide) {
			sb.addParticipantInList(member->getId(), member->getStatus().initialSpeed, false);
		}
		for (unsigned i = 0; i < contenderBySide.size(); ++i) {
			sb.addParticipantInList(i, contenderBySide.at(i)->getStatus().initialSpeed, true);
		}
	};
}
void
fighting_pit::set_player_readiness(const std::string& user_name)
{
	SPDLOG_INFO("[fp:{}] : Player '{}' is setting readiness", _arena_id, user_name);
	if (_party_teams.setPartyReadiness(user_name)) {
		_progress = progress::ON_GOING;
	}
}
std::pair<bool, std::optional<TargetType>>
fighting_pit::check_and_retrieve_target(const std::string& user, const TeamMemberSPtr& member, const PlayerAction& action)
{
	auto targetType = _chai_ptr->eval<data::Targeting>(
			chai::util::getAccessAllyAction(user, member->getName(), action.actionName).append(".requireTarget();"));
	std::optional<TargetType> target;

	// Check if the target is appropriate
	if (targetType == data::Targeting::SELF) {
		if (!action.allyTarget.empty() || !action.contenderTarget.empty()) {
			SPDLOG_WARN("[fp:{}] : Action {} target type is SELF, but Player {} tried to execute it with targets set",
					action.actionName, user);
			return std::pair(false, target);
		}
		target = std::nullopt;
	}
	else if (targetType == data::Targeting::ENNEMY) {
		if (!action.allyTarget.empty() || action.contenderTarget.empty()) {
			SPDLOG_WARN("[fp:{}] Action {} target type is ENNEMY, but Player {} tried to execute it with ally targets set",
					_arena_id, action.actionName, user);
			return std::pair(false, target);
		}
		target = ContenderTargetId{action.contenderTarget.at(0)};
	}
	else if (targetType == data::Targeting::ENNEMIES) {
		if (!action.allyTarget.empty() || action.contenderTarget.empty()) {
			SPDLOG_WARN("[fp:{}] : Action {} target type is ENNEMIES, but Player {} tried to execute it with ally targets set",
					_arena_id, action.actionName, user);
			return std::pair(false, target);
		}
		target = ContendersTargetsIds{std::move(action.contenderTarget)};
	}
	else if (targetType == data::Targeting::ALLY) {
		if (action.allyTarget.empty() || !action.contenderTarget.empty()) {
			SPDLOG_WARN("[fp:{}] : Action {} target type is ALLY, but Player {} tried to execute it with contender targets set",
					_arena_id, action.actionName, user);
			return std::pair(false, target);
		}
		target = AllyTargetId{action.allyTarget.at(0)};
	}
	else if (targetType == data::Targeting::ALLIES) {
		if (action.allyTarget.empty() || !action.contenderTarget.empty()) {
			SPDLOG_WARN("[fp:{}] : Action {} target type is ALLIES, but Player {} tried to execute it with contender targets set",
					_arena_id, action.actionName, user);
			return std::pair(false, target);
		}
		target = AlliesTargetsIds{std::move(action.allyTarget)};
	}
//	else if (targetType == data::Targeting::ALLY_AND_ENNEMY) {}
//	else if (targetType == data::Targeting::ALLY_OR_ENNEMY) {}
	else {
		SPDLOG_WARN("NOT IMPLEMENTED YET");
		return std::pair(false, target);
	}
	if (!all_in(action.allyTarget, _party_teams.get_party_team_of_player(user).getTeamMembers(),
			[](const TeamMemberSPtr& tm) { return tm->getId(); })) {
		SPDLOG_WARN("[fp:{}] : Player {} tried to target a non-existing Ally", _arena_id, user);
		return std::pair(false, target);
	}
	if (!all_in(action.contenderTarget, _contenders.get_contenders(),
			[](const fighting_contender_sptr& c) { return c->get_id(); })) {
		SPDLOG_WARN("[fp:{}] : Player {} tried to target a non-existing Contender, Targets are : ", _arena_id, user);
		return std::pair(false, target);
	}
	return std::pair(true, target);
}
bool
fighting_pit::add_contender(const std::shared_ptr<fighting_contender>& fc)
{
	if (_contenders.addContender(fc)) {
		_layout_mapping.addActiveContender();
		return true;
	}
	return false;
}

zmq::message_t
fighting_pit::make_winner_notification() const
{
	flatbuffer_generator fb;
	auto[data, size] = fb.generateEndBattle(true, *_rewards);
	return zmq::message_t(data, size);
}

zmq::message_t
fighting_pit::make_looser_notification() const
{
	flatbuffer_generator fb;
	auto[data, size] = fb.generateEndBattle(false, {});
	return zmq::message_t(data, size);
}

}
