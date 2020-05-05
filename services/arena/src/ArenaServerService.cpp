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
#include <functional>
#include <chaiscript/chaiscript.hpp>
#include <zmq_addon.hpp>

#include <flatbuffers/flatbuffers.h>
#include <FightingPitEncounter_generated.h>
#include <ArenaServerAuth_generated.h>
#include <ArenaServerValidateAuth_generated.h>
#include <ArenaFightAction_generated.h>

#include <network/DBConnector.hh>
#include <fightingPit/FightingPitAnnouncer.hh>

#include <ArenaServerContext.hh>
#include <FlatbufferGenerator.hh>
#include <FightingHistoryManager.hh>
#include "ArenaServerService.hh"

// anonymous namespace used for utility function to extract data from flatbuffer
namespace {

template<typename T>
[[nodiscard]] bool
verifyBuffer(const void* fbBuffer, uint size)
{
	auto v = flatbuffers::Verifier(static_cast<const uint8_t*>(fbBuffer), size);
	return v.VerifyBuffer<T>();
}

[[nodiscard]] fys::arena::FightingPit::Level
translateLevelFromFlatbuffer(const fys::fb::Level& level)
{
	switch (level) {
		case fys::fb::Level_EASY:return fys::arena::FightingPit::Level::EASY;
		case fys::fb::Level_MEDIUM:return fys::arena::FightingPit::Level::MEDIUM;
		case fys::fb::Level_HARD:return fys::arena::FightingPit::Level::HARD;
		default:return fys::arena::FightingPit::Level::EASY;
	}
}

[[nodiscard]] fys::arena::AwaitingPlayerArena
createAwaitingPlayer(const fys::fb::FightingPitEncounter* binary)
{
	std::optional<fys::arena::AwaitingArena> awaitingArena = std::nullopt;

	// if fighting_pit_id is 0, a new arena has to be generated, in which case more data are extracted
	if (!binary->fighting_pit_id()) {
		awaitingArena = fys::arena::AwaitingArena{
				binary->world_server_id()->str(),
				binary->disable_join(),
				binary->is_ambush(),
				binary->id_encounter(),
				translateLevelFromFlatbuffer(binary->level_encounter())
		};
	}
	return fys::arena::AwaitingPlayerArena{
			binary->user_name()->str(),
			binary->token_auth()->str(),
			binary->fighting_pit_id(),
			std::move(awaitingArena)
	};
}

}

namespace fys::arena {

ArenaServerService::ArenaServerService(const ArenaServerContext& ctx)
		:
		_ctx(ctx),
		_cache(ctx.getPathLocalStorageCache(), ctx.getPathSourceCache()),
		_dbConnector(std::make_unique<network::DBConnector>(ctx))
{
	_connectionHandler.setupConnectionManager(ctx);
	_workerService.setupConnectionManager(ctx);
}

void
ArenaServerService::runServerLoop() noexcept
{
	SPDLOG_INFO("ArenaServer loop started");

	auto t = std::thread([this] { _workerService.startFightingPitsLoop(); });
	while (true) {
		_connectionHandler.pollAndProcessMessageFromDispatcher(
				// World Server inform the Arena that a new awaited player is coming (create or join an existing fighting pit)
				[this](zmq::message_t&& identityWs, zmq::message_t&& worldServerMessage) {

					// In case of a saturation of the server, return an error to the dispatcher
					if (isSaturated()) {
						sendSaturatedErrorMsg(std::move(identityWs));
						return;
					}

					if (!verifyBuffer<fb::FightingPitEncounter>(worldServerMessage.data(), worldServerMessage.size())) {
						SPDLOG_ERROR("Wrongly formatted FightingPitEncounter buffer");
						return;
					}
					const auto* binary = fys::fb::GetFightingPitEncounter(worldServerMessage.data());

					AwaitingPlayerArena apa = createAwaitingPlayer(binary);

					if (auto[exist, joinable] = _workerService.fightingPitExistAndJoinable(apa.fightingPitId);
							!apa.hasToBeGenerated() && !exist && !joinable) {
						SPDLOG_ERROR("Player {} can't be awaited to register on non-existing({})/joinable({})"
									 " fighting pit {}", exist, joinable, apa.namePlayer, apa.fightingPitId);
						return;
					}

					// register player incoming into arena instance with token given by worldserver used as key
					const auto &[elem, isRegistered] = _awaitingArena.insert({binary->token_auth()->str(), std::move(apa)});

					if (isRegistered) {
						forwardReplyToDispatcher(std::move(identityWs), elem->second);
						SPDLOG_INFO("A new awaited player is added '{}'", binary->token_auth()->str());
					}
				});

		_workerService.pollAndProcessMessageFromPlayer(
				// Authentication handler : Player try to create/join a fighting pit. The player has to be awaited
				[this](zmq::message_t&& idtPlayer, zmq::message_t&& authMessage) {
					if (!verifyBuffer<fb::ArenaServerValidateAuth>(authMessage.data(), authMessage.size())) {
						SPDLOG_ERROR("Wrongly formatted ArenaServerValidateAuth buffer");
						return;
					}

					const auto* binary = fys::fb::GetArenaServerValidateAuth(authMessage.data());
					const std::string tokenAuth = binary->token_auth()->str();
					const std::string userName = binary->user_name()->str();
					const auto &[isAwaited, playerAwaitedIt] = isPlayerAwaited(userName, tokenAuth, binary->fighting_pit_id());
					unsigned fightingPitId = playerAwaitedIt->second.fightingPitId;

					if (!isAwaited) {
						SPDLOG_WARN("Player {} tried to authenticate on Arena server {} fighting pit {} "
									"without being awaited.", userName, _ctx.get().getServerCode(), fightingPitId);
						return;
					}
					SPDLOG_INFO("Awaited Player {} login", userName);

					if (playerAwaitedIt->second.hasToBeGenerated()) {
						fightingPitId = createNewFightingPit(playerAwaitedIt->second);
						_awaitingArena.erase(playerAwaitedIt); // remove player from awaited player
						if (fightingPitId == FightingPit::CREATION_ERROR) return;
					}
					else {
						if (auto[exist, isJoinable] = _workerService.fightingPitExistAndJoinable(binary->fighting_pit_id());
								!exist || !isJoinable) {
							SPDLOG_ERROR("Player {} can't be join on non-existing({})/joinable({}) fighting pit {}",
									exist, isJoinable, userName, binary->fighting_pit_id());
							return;
						}

						auto pt = _dbConnector->retrievePartyTeam(userName);
						_workerService.playerJoinFightingPit(fightingPitId, std::move(pt), _cache);
						_awaitingArena.erase(playerAwaitedIt); // remove player from awaited player
					}
					SPDLOG_INFO("Awaited player '{}' has logged in fighting pit of id: '{}'", userName, fightingPitId);
					_workerService.upsertPlayerIdentifier(fightingPitId, userName, idtPlayer.to_string());
					_workerService.sendMsgNewArrivingTeam(fightingPitId, userName);
				},

				// InGame handler: Player is sending actions to feed pendingActions queue of their characters
				[this](zmq::message_t&& idtPlayer, const zmq::message_t& intermediate, zmq::message_t&& playerMsg) {
					if (!verifyBuffer<fb::ArenaServerValidateAuth>(intermediate.data(), intermediate.size())) {
						SPDLOG_ERROR("Wrongly formatted ArenaServerValidateAuth (auth frame) buffer");
						return;
					}
					if (!verifyBuffer<fb::ArenaFightAction>(playerMsg.data(), playerMsg.size())) {
						SPDLOG_ERROR("Wrongly formatted ArenaFightAction buffer");
						return;
					}

					const auto* authFrame = fys::fb::GetArenaServerValidateAuth(intermediate.data());
					const std::string userName = authFrame->user_name()->str();
					auto fp = _workerService.getAuthenticatedPlayerFightingPit(userName,
							authFrame->token_auth()->str(),
							authFrame->fighting_pit_id());

					if (!fp) {
						SPDLOG_WARN("Player {}:{} is not authenticated.", userName, authFrame->token_auth()->str());
						return;
					}
					_workerService.upsertPlayerIdentifier(authFrame->fighting_pit_id(), userName, idtPlayer.str());

					const auto* frame = fys::fb::GetArenaFightAction(playerMsg.data());
					std::string action = frame->actionId()->str();

					if (frame->memberId() == READY_ACTION_ID && action == READY_ACTION) {
						fp->get().setPlayerReadiness(userName);
					}
					else if (fp->get().isBattleOnGoing()) {
						fp->get().forwardActionToTeamMember(userName, createPlayerAction(std::move(action), frame));
					}
					else {
						SPDLOG_WARN("An action {} action has been received from player {} "
									"while the battle isn't on-going", action, userName);
					}
				});
	}
	t.join();
}

PlayerAction
ArenaServerService::createPlayerAction(std::string&& action, const fb::ArenaFightAction* frame) const
{
	std::vector<uint> contenders;
	std::vector<uint> allies;

	if (frame->targetId_contender()) {
		contenders.reserve(frame->targetId_contender()->size());
		std::move(frame->targetId_contender()->begin(), frame->targetId_contender()->end(), std::back_inserter(contenders));
	}
	if (frame->targetId_ally()) {
		allies.reserve(frame->targetId_ally()->size());
		std::move(frame->targetId_ally()->begin(), frame->targetId_ally()->end(), std::back_inserter(allies));
	}
	return PlayerAction{
			frame->memberId(),
			std::move(action),
			std::move(contenders),
			std::move(allies)};
}

bool
ArenaServerService::isSaturated() const noexcept
{
	const unsigned numberArenaManagedByServer = _awaitingArena.size() + _workerService.getNumberBattleRunning();
	return numberArenaManagedByServer > _ctx.get().getBattleThreshold();
}

std::pair<bool, const std::unordered_map<std::string, AwaitingPlayerArena>::const_iterator>
ArenaServerService::isPlayerAwaited(const std::string& name, const std::string& token, unsigned idFightingPit) const noexcept
{
	if (const auto it = _awaitingArena.find(token);
			it != _awaitingArena.end() && it->second.namePlayer == name && it->second.fightingPitId == idFightingPit)
		return {true, it};
	return {false, _awaitingArena.cend()};
}

void
ArenaServerService::forwardReplyToDispatcher(zmq::message_t&& idtWs, const fys::arena::AwaitingPlayerArena& awaitArena) noexcept
{
	flatbuffers::FlatBufferBuilder fbb;
	auto asaFb = fys::fb::CreateArenaServerAuth(
			fbb,
			fbb.CreateString(awaitArena.namePlayer),
			fbb.CreateString(awaitArena.token),
			fbb.CreateString(_ctx.get().getHostname()),
			fbb.CreateString(_ctx.get().getConnectionString()),
			_ctx.get().getPort(),
			awaitArena.gen ? fbb.CreateString(awaitArena.gen->serverCode) : fbb.CreateString(""));
	fys::fb::FinishArenaServerAuthBuffer(fbb, asaFb);
	zmq::multipart_t msg;
	msg.add(std::move(idtWs));
	msg.addmem(fbb.GetBufferPointer(), fbb.GetSize());
	_connectionHandler.sendMessageToDispatcher(std::move(msg));
}

unsigned
ArenaServerService::createNewFightingPit(const AwaitingPlayerArena& awaited) noexcept
{
	FightingPitAnnouncer fpa(_cache);

	fpa.setDifficulty(awaited.gen->levelFightingPit);
	fpa.setEncounterId(awaited.gen->encounterId);
	fpa.enforceAmbush(awaited.gen->isAmbush);
	fpa.setCreatorUserName(awaited.namePlayer);
	fpa.setCreatorUserToken(awaited.token);
	fpa.setCreatorTeamParty(_dbConnector->retrievePartyTeam(awaited.namePlayer));
	fpa.setJoinDisabled(awaited.gen->isJoinDisabled);

	unsigned id = _workerService.addFightingPit(
			fpa.buildFightingPit(_ctx.get().getEncounterContext(), awaited.gen->serverCode));

	if (id != FightingPit::CREATION_ERROR) {
		SPDLOG_INFO("New fighting pit of id '{}' is created name '{}', token '{}' lvl '{}' "
					"encounter_id '{}' isAmbush '{}' wsCode '{}' isJoinDisabled '{}'",
				id, awaited.namePlayer, awaited.token, awaited.gen->levelFightingPit, awaited.gen->encounterId,
				awaited.gen->isAmbush, awaited.gen->serverCode, awaited.gen->isJoinDisabled);
	}
	return id;
}

void
ArenaServerService::sendSaturatedErrorMsg(zmq::message_t&& identity)
{
	SPDLOG_WARN("Arena Server Service is saturated and thus can't process new incoming fighting pit");
	auto[error, size] = fys::arena::FlatbufferGenerator().generateErrorSaturated(_ctx.get().getServerCode());
	_workerService.directSendMessageToPlayer(std::move(identity), zmq::message_t(error, size));
}

} // namespace fys::arena