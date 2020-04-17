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
#include <chrono>

#include <chaiscript/chaiscript.hpp>

#include <fightingPit/FightingPit.hh>
#include <network/WorkerService.hh>
#include <ArenaServerContext.hh>

namespace fys::arena {

WorkerService::WorkerService()
		:
		_ctx(1), _workerRouter(_ctx, zmq::socket_type::router), _currentArenaId(0) { }

void
WorkerService::setupConnectionManager(const fys::arena::ArenaServerContext& ctx) noexcept
{
	_workerRouter.bind(ctx.getPlayerBindingString());
}

void
WorkerService::startFightingPitsLoop()
{
	SPDLOG_INFO("WorkerService FightingPit game loops started");

	using namespace std::chrono_literals;
	while (true) {
		auto now = std::chrono::system_clock::now();

		if (!_arenaInstances.empty()) {
			for (auto &[id, fp] : _arenaInstances) {
				fp->continueBattle(now);
			}
			cleanUpFinishedBattles();
		}
		std::this_thread::sleep_for(1000ms); // todo sleep something smart
	}
}

unsigned
WorkerService::addFightingPit(std::unique_ptr<FightingPit> fp)
{
	if (!fp) {
		SPDLOG_ERROR("Cannot add fighting pit in WorkerService");
		return FightingPit::CREATION_ERROR;
	}
	if (_arenaInstances.size() >= std::numeric_limits<decltype(_currentArenaId)>::max()) {
		SPDLOG_ERROR("Cannot add fighting pit in WorkerService (worker full)");
		return FightingPit::CREATION_ERROR;
	}
	while (++_currentArenaId != 0 && _arenaInstances.find(_currentArenaId) != _arenaInstances.end());

	fp->setArenaId(_currentArenaId);
	_arenaInstances[_currentArenaId] = std::move(fp);
	return _currentArenaId;
}

void
WorkerService::cleanUpFinishedBattles()
{
	std::vector<unsigned> idToRemove;
	idToRemove.reserve(_arenaInstances.size());
	for (auto &[id, fp] : _arenaInstances) {
		if (fp->isBattleOver()) {
			idToRemove.emplace_back(id);
		}
	}
	for (unsigned id : idToRemove) {
		_arenaInstances.erase(id);
		_arenaIdOnIdentifier.erase(id);
	}

}

void
WorkerService::playerJoinFightingPit(unsigned fightingPitId, std::unique_ptr<PartyTeam> pt, cache::Cml& cml)
{
	auto it = _arenaInstances.find(fightingPitId);
	if (it != _arenaInstances.end()) {
		if (it->second->isJoinable()) {
			it->second->addPartyTeamAndRegisterActions(std::move(pt), cml);
		}
		else {
			SPDLOG_INFO("User {} can't join fighting pit of id {} as the battle already started.", pt->getUserName(), fightingPitId);
		}
	}
	else {
		SPDLOG_ERROR("PartyTeam of user {} can't join fighting pit of id {}", pt->getUserName(), fightingPitId);
	}
}

std::optional<std::reference_wrapper<FightingPit>>
WorkerService::getAuthenticatedPlayerFightingPit(const std::string& name, const std::string& token, unsigned fightingPitId)
{
	if (auto it = _arenaInstances.find(fightingPitId);
			(it != _arenaInstances.end() && it->second->isPlayerParticipant(name, token))) {
		return *it->second;
	}
	SPDLOG_WARN("Request received from {}:{} for arena id {}, but arena isn't defined", name, token, fightingPitId);
	return std::nullopt;
}

void
WorkerService::upsertPlayerIdentifier(unsigned fightingPitId, std::string userName, std::string idtPlayer)
{
	auto& identifiers = _arenaIdOnIdentifier[fightingPitId];
	if (auto it = std::find_if(identifiers.begin(), identifiers.end(), [&userName](const auto& idt) {
			return idt.userName == userName;
		}); it != identifiers.end()) {
		it->identifier = std::move(idtPlayer);
	}
	else {
		identifiers.emplace_back(PlayerIdentifier{std::move(userName), std::move(idtPlayer)});
	}
}

void
WorkerService::broadCastNewArrivingTeam(unsigned fightingPitId, const std::string& userName) noexcept
{
	zmq::multipart_t msg;
	const PartyTeam& pt = _arenaInstances.at(fightingPitId)->getPartyTeamOfPlayer(userName);

	broadcastMsg(fightingPitId, msg);
}

const std::string&
WorkerService::retrievePlayerIdentifier(unsigned fightingPitId, const std::string& userName)
{
	auto identifiersIt = _arenaIdOnIdentifier.find(fightingPitId);
	if (identifiersIt == _arenaIdOnIdentifier.end())
		return userName;

	auto it = std::find_if(identifiersIt->second.begin(), identifiersIt->second.end(), [&userName](const auto& ident) {
		return ident.userName == userName;
	});

	if (it != identifiersIt->second.end()) {
		return it->identifier;
	}
	return userName;
}

void
WorkerService::broadcastMsg(unsigned fightingPitId, zmq::multipart_t& msg)
{
	auto identifiersIt = _arenaIdOnIdentifier.find(fightingPitId);
	if (identifiersIt == _arenaIdOnIdentifier.end())
		return;
	unsigned identifierIndex = msg.size();

	msg.add({});
	for (const auto&[userName, identifier] : identifiersIt->second) {
		msg.at(identifierIndex).rebuild(identifier.data(), identifier.size());
		if (!msg.send(_workerRouter))
			SPDLOG_ERROR("fightingPit of id {} : Message has not been correctly sent to {}, {}", fightingPitId, userName, identifier);
	}
}

std::pair<bool, bool>
WorkerService::fightingPitExistAndJoinable(unsigned int fightingPitId) const noexcept
{
	auto it = _arenaInstances.find(fightingPitId);
	return std::pair(it != _arenaInstances.cend(), it != _arenaInstances.cend() && it->second->isJoinable());
}


} // namespace fys::arena
