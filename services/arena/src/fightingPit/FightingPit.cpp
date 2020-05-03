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
#include <ChaiRegister.hh>

#include <algorithm/algorithm.hh>
#include <network/WorkerService.hh>

#include <fightingPit/contender/FightingContender.hh>
#include <fightingPit/FightingPit.hh>
#include <fightingPit/team/TeamMember.hh>

namespace {
std::chrono::milliseconds
retrieveTimeInterludeFromLevelDegree(fys::arena::FightingPit::Level level)
{
	switch (level) {
		case fys::arena::FightingPit::Level::EASY:return fys::arena::interval::EASY;
		case fys::arena::FightingPit::Level::MEDIUM :return fys::arena::interval::MEDIUM;
		case fys::arena::FightingPit::Level::HARD :return fys::arena::interval::HARD;
		default:SPDLOG_ERROR("Incorrect level");
			return std::chrono::milliseconds{0};
	}
}
}

namespace fys::arena {

FightingPit::FightingPit(std::string creatorUserName, fys::arena::FightingPit::Level levelFightingPit)
		:
		_progress(Progress::ON_HOLD),
		_levelFightingPit(levelFightingPit),
		_timeInterlude(retrieveTimeInterludeFromLevelDegree(_levelFightingPit)),
		_layoutMapping(_contenders, _partyTeams),
		_creatorUserName(std::move(creatorUserName)),
		_chaiPtr(ChaiRegister::createChaiInstance(_contenders, _partyTeams, _layoutMapping)),
		_rewards(std::make_unique<Rewards>()) { }

void
FightingPit::continueBattle(const std::chrono::system_clock::time_point& now)
{
	for (auto& battle : _sideBattles) {
		if (battle.empty()) { // if battle side is empty, ignore it
			continue;
		}
		auto currentParticipant = battle.getCurrentParticipantTurn(now, _timeInterlude);

		if (currentParticipant.isContender) {
			// non-playable character (enemy NPC)
			_contenders.executeContenderAction(currentParticipant);
		}
		else {
			// character of a player
			_partyTeams.executeAllyAction(currentParticipant, _contenders, _chaiPtr);
		}
		battle.eraseFinishedAlterationAndDeadCharactersFromTurnList();
	}
	// execute awaited movements
	_layoutMapping.executeMovements(_sideBattles);

	_progress = updateProgressStatus();
}

FightingPit::Progress
FightingPit::updateProgressStatus()
{
	if (_partyTeams.allDead()) {
		return Progress::CONTENDER_WIN;
	}
	else if (_contenders.allDead()) {
		return Progress::ALLY_WIN;
	}
	return Progress::ON_GOING;
}

void
FightingPit::forwardActionToTeamMember(const std::string& user, PlayerAction action)
{
	auto member = _partyTeams.getSpecificTeamMemberById(user, action.idMember);
	if (!member) {
		SPDLOG_ERROR("Trying to forward a message to team member {} owned by {} whom doesn't exist",
				action.idMember, user);
		return;
	}
	try {
		if (chai::util::memberHasActionRegistered(*_chaiPtr, user, member->getName(), action.actionName)) {
			SPDLOG_WARN("Player {} tried to register Action {} which isn't registered for member {}",
					user, action.actionName, member->getName());
			return;
		}
		auto[success, target] = this->checkAndRetrieveTarget(user, member, action);
		if (success) {
			member->addPendingAction(std::move(action.actionName), std::move(target));
		}
	}
	catch (const std::exception& e) {
		SPDLOG_ERROR("An error occurred when checking if an action was doable from user {}: {}", user, e.what());
	}
}

void
FightingPit::addAuthenticatedUser(std::string userName, std::string userToken)
{
	_authenticatedPlayers.push_back({std::move(userName), std::move(userToken)});
}

bool
FightingPit::isPlayerParticipant(const std::string& name, const std::string& token) const
{
	return std::any_of(_authenticatedPlayers.begin(), _authenticatedPlayers.end(),
			[&name, &token](auto& authPlayer) { return authPlayer.name == name && authPlayer.token == token; });
}

void
FightingPit::addPartyTeamAndRegisterActions(std::unique_ptr<PartyTeam> pt, cache::Cml& cml)
{
	ChaiRegister::loadAndRegisterActionPartyTeam(*_chaiPtr, cml, *pt);
	_layoutMapping.addActivePartyTeam(*pt);
	_partyTeams.addPartyTeam(std::move(pt));
	std::sort(_sideBattles.begin(), _sideBattles.end(),
			[this](auto& lhs, auto& rhs) {
				return _layoutMapping.activeCharactersOnSide(lhs.getSide()) < _layoutMapping.activeCharactersOnSide(rhs.getSide());
			});
}

// Initialization methods used by FightingPitAnnouncer

void
FightingPit::initializeSideBattles()
{
	_sideBattles.reserve(HexagonSide::SIDE_NUMBER);

	// Hexagon A creation
	_sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::A_N);
	_sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::A_NE);
	_sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::A_SE);
	_sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::A_S);
	_sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::A_SW);
	_sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::A_NW);

	// Hexagon B creation
	_sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::B_N);
	_sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::B_NE);
	_sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::B_SE);
	_sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::B_S);
	_sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::B_SW);
	_sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::B_NW);

	// Hexagon C creation
	_sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::C_N);
	_sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::C_NE);
	_sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::C_SE);
	_sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::C_S);
	_sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::C_SW);
	_sideBattles.emplace_back(_contenders, _partyTeams, HexagonSide::Orientation::C_NW);

	initializePriorityListInSidesBattle();
}

void
FightingPit::initializePriorityListInSidesBattle()
{
	for (auto& sb : _sideBattles) {
		const auto& memberBySide = _partyTeams.getMembersBySide(sb.getSide());
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
FightingPit::setPlayerReadiness(const std::string& userName)
{
	if (_partyTeams.setPartyReadiness(userName)) {
		_progress = Progress::ON_GOING;
	}
}
std::pair<bool, std::optional<TargetType>>
FightingPit::checkAndRetrieveTarget(const std::string& user, const TeamMemberSPtr& member, const PlayerAction& action)
{
	auto targetType = _chaiPtr->eval<data::Targeting>(
			chai::util::getAccessAllyAction(user, member->getName(), action.actionName)
					.append(".requireTarget();"));
	std::optional<TargetType> target;

	// Check if the target is appropriate
	if (targetType == data::Targeting::SELF) {
		if (!action.allyTarget.empty() || !action.contenderTarget.empty()) {
			SPDLOG_WARN("Action {} target type is SELF, but Player {} tried to execute it with targets set",
					action.actionName, user);
			return std::pair(false, target);
		}
		target = std::nullopt;
	}
	else if (targetType == data::Targeting::ENNEMY) {
		if (!action.allyTarget.empty() && action.contenderTarget.empty()) {
			SPDLOG_WARN("Action {} target type is ENNEMY, but Player {} tried to execute it with ally targets set",
					action.actionName, user);
			return std::pair(false, target);
		}
		target = ContenderTargetId{action.allyTarget.at(0)};
	}
	else if (targetType == data::Targeting::ENNEMIES) {
		if (!action.allyTarget.empty() && action.contenderTarget.empty()) {
			SPDLOG_WARN("Action {} target type is ENNEMIES, but Player {} tried to execute it with ally targets set",
					action.actionName, user);
			return std::pair(false, target);
		}
		target = ContendersTargetsIds{std::move(action.contenderTarget)};
	}
	else if (targetType == data::Targeting::ALLY) {
		if (action.allyTarget.empty() && !action.contenderTarget.empty()) {
			SPDLOG_WARN("Action {} target type is ALLY, but Player {} tried to execute it with contender targets set",
					action.actionName, user);
			return std::pair(false, target);
		}
		target = AllyTargetId{action.allyTarget.at(0)};
	}
	else if (targetType == data::Targeting::ALLIES) {
		if (action.allyTarget.empty() && !action.contenderTarget.empty()) {
			SPDLOG_WARN("Action {} target type is ALLIES, but Player {} tried to execute it with contender targets set",
					action.actionName, user);
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

	if (!all_in(action.allyTarget, _partyTeams.getPartyTeamOfPlayer(user).getTeamMembers(),
			[](const TeamMemberSPtr& tm) { return tm->getId(); })) {
		SPDLOG_WARN("Player {} tried to target a non-existing Ally", user);
		return std::pair(false, target);
	}

	if (!all_in(action.contenderTarget, _contenders.getContenders(),
			[](const FightingContenderSPtr& c) { return c->getId(); })) {
		SPDLOG_WARN("Player {} tried to target a non-existing Contender", user);
		return std::pair(false, target);
	}

	return std::pair(true, target);
}
bool
FightingPit::addContender(const std::shared_ptr<FightingContender>& fc)
{
	if (_contenders.addContender(fc)) {
		_layoutMapping.addActiveContender();
		return true;
	}
	return false;
}

}
