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

#include <FightingPitState_generated.h>

#include <fightingPit/FightingPit.hh>

#include <FightingHistoryManager.hh>
#include <network/WorkerService.hh>
#include <ArenaServerContext.hh>
#include <FlatbufferGenerator.hh>

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
				if (fp->isBattleOnGoing()) {
					fp->continueBattle(now);
				}
				else if (!fp->isJoinable()) {
					fp->notifyEndStatus(broadcastMsgHandler(id));
				}
			}
			cleanUpFinishedBattles();
		}
		std::this_thread::sleep_for(100ms); // todo sleep something smart
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
	ChaiRegister::registerNetworkCommands(*fp->getChaiPtr(), broadcastMsgHandler(fp->getId()));
	fp->setArenaId(_currentArenaId);
	_arenaInstances.insert(std::pair(_currentArenaId, std::move(fp)));
	_arenaIdOnIdentifier.insert(std::pair(_currentArenaId, std::vector<PlayerIdentifier>{}));
	FightingHistoryManager::createHistoric(*fp, 1);
	return _currentArenaId;
}

void
WorkerService::cleanUpFinishedBattles()
{
	std::vector<unsigned> idsToRemove;
	idsToRemove.reserve(_arenaInstances.size());
	for (auto &[id, fp] : _arenaInstances) {
		if (fp->isBattleOver()) {
			idsToRemove.emplace_back(id);
		}
	}
	for (unsigned id : idsToRemove) {
		FightingHistoryManager::save(id);
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
			SPDLOG_INFO("User {} can't join fighting pit of id {} as the battle already started.",
					pt->getUserName(), fightingPitId);
		}
	}
	else {
		SPDLOG_ERROR("PartyTeam of user {} can't join fighting pit of id {}",
				pt->getUserName(), fightingPitId);
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
	auto itIdt = _arenaIdOnIdentifier.find(fightingPitId);
	if (itIdt == _arenaIdOnIdentifier.end())
		return;

	auto& identifiers = itIdt->second;
	if (auto it = std::find_if(identifiers.begin(), identifiers.end(), [&userName](const auto& idt) {
			return idt.userName == userName;
		}); it != identifiers.end()) {
		it->identifier = std::move(idtPlayer);
	}
	else {
		identifiers.emplace_back(PlayerIdentifier{std::move(userName), std::move(idtPlayer)});
	}
}

std::pair<bool, bool>
WorkerService::fightingPitExistAndJoinable(unsigned int fightingPitId) const noexcept
{
	auto it = _arenaInstances.find(fightingPitId);
	return std::pair(it != _arenaInstances.cend(), it != _arenaInstances.cend() && it->second->isJoinable());
}

void
WorkerService::sendMsgNewArrivingTeam(unsigned fpId, const std::string& userName) noexcept
{
	FlatbufferGenerator fg;

	{
		auto[fbMsg, size] = fg.generateFightingPitState(*_arenaInstances.at(fpId));
		sendMessageToPlayer(fpId, userName, zmq::message_t(fbMsg, size));
	}
	{
		auto[fbMsg, size] = fg.generatePartyTeamStatus(_arenaInstances.at(fpId)->getPartyTeamOfPlayer(userName));
		broadcastMsg(fpId, zmq::message_t(fbMsg, size), userName);
	}
}

void
WorkerService::directSendMessageToPlayer(zmq::message_t&& identity, zmq::message_t&& msg)
{
	zmq::multipart_t toSend;
	toSend.add(std::move(identity));
	toSend.add(std::move(msg));
	if (!toSend.send(_workerRouter)) {
		SPDLOG_ERROR("Failure to send directly the message: {}", toSend.str());
	}
}

bool
WorkerService::sendMessageToPlayer(unsigned fpId, const std::string& userName, zmq::message_t&& msg)
{
	const std::string& identifier = retrievePlayerIdentifier(fpId, userName);
	if (identifier == userName) {
		SPDLOG_ERROR("Cannot send a message in fightingPit {} to player {}. {}", fpId, userName, msg.str());
		return false;
	}
	zmq::multipart_t toSend;
	toSend.addstr(identifier);
	toSend.add(std::move(msg));
	SPDLOG_DEBUG("Send message to player {} with identifier {}", userName, identifier);
	if (!toSend.send(_workerRouter)) {
		SPDLOG_ERROR("Failure to send message to player {} in fightingPit {}.", userName, fpId);
		return false;
	}
	return true;
}

bool
WorkerService::broadcastMsg(unsigned fpId, zmq::message_t&& msg, const std::string& except)
{
	const auto identifiersIt = _arenaIdOnIdentifier.find(fpId);
	if (identifiersIt == _arenaIdOnIdentifier.end())
		return false;
	zmq::multipart_t toSend;

	toSend.add({}); // add empty frame (identifier frame at index 0)
	toSend.add(std::move(msg)); // add content

	for (const auto&[userName, identifier] : identifiersIt->second) {
		if (userName == except)
			continue;
		toSend.at(0).rebuild(identifier.data(), identifier.size());
		SPDLOG_DEBUG("Send message to player {} with identifier {}", userName, identifier);
		if (!toSend.send(_workerRouter)) {
			SPDLOG_ERROR("fightingPit of id {} : Message has not been correctly sent to {}, {}", fpId, userName, identifier);
			return false;
		}
	}
	return true;
}

const std::string&
WorkerService::retrievePlayerIdentifier(unsigned fpId, const std::string& userName)
{
	auto identifiersIt = _arenaIdOnIdentifier.find(fpId);
	if (identifiersIt == _arenaIdOnIdentifier.end()) {
		SPDLOG_WARN("Trying to retrieve player identifier on non-existing fighting pit of id {}", fpId);
		return userName;
	}

	auto it = std::find_if(identifiersIt->second.begin(), identifiersIt->second.end(), [&userName](const auto& ident) {
		return ident.userName == userName;
	});
	if (it == identifiersIt->second.end()) {
		SPDLOG_WARN("Trying to retrieve non-existing player identifier {} in fighting pit of id {}", userName, fpId);
		return userName;
	}
	return it->identifier;
}

} // namespace fys::arena
