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

#include <network/DBConnector.hh>
#include <fightingPit/FightingPitAnnouncer.hh>

#include <ArenaServerContext.hh>
#include "ArenaServerService.hh"

// anonymous namespace used for utility function to extract data from flatbuffer
namespace {

fys::arena::FightingPit::Level
translateLevelFromFlatbuffer(const fys::fb::Level& level)
{
	switch (level) {
		case fys::fb::Level_EASY:return fys::arena::FightingPit::Level::EASY;
		case fys::fb::Level_MEDIUM:return fys::arena::FightingPit::Level::MEDIUM;
		case fys::fb::Level_HARD:return fys::arena::FightingPit::Level::HARD;
		default:return fys::arena::FightingPit::Level::EASY;
	}
}

fys::arena::AwaitingPlayerArena
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
						// todo return an error to the dispatcher containing all the incoming message for it to be forwarded to another server
						return;
					}

					const auto* binary = fys::fb::GetFightingPitEncounter(worldServerMessage.data());

					AwaitingPlayerArena apa = createAwaitingPlayer(binary);

					if (auto[exist, joinable] = _workerService.fightingPitExistAndJoinable(apa.fightingPitId);
							!apa.hasToBeGenerated() && !exist && !joinable) {
						spdlog::error("Player {} can't be awaited to register on non-existing({})/joinable({})"
									  " fighting pit {}", exist, joinable, apa.namePlayer, apa.fightingPitId);
						return;
					}

					// register player incoming into arena instance with token given by worldserver used as key
					const auto &[elem, playerHasBeenCorrectlyRegistered] = _awaitingArena.insert({binary->token_auth()->str(), std::move(apa)});

					if (playerHasBeenCorrectlyRegistered) {
						forwardReplyToDispatcher(std::move(identityWs), elem->second);
						spdlog::info("A new awaited player is incoming {}", binary->token_auth()->str());
					}
				});

		_workerService.pollAndProcessMessageFromPlayer(
				// Authentication handler : Player try to create/join a fighting pit. The player has to be awaited
				[this](zmq::message_t&& idtPlayer, zmq::message_t&& authMessage) {
					const auto* binary = fys::fb::GetArenaServerValidateAuth(authMessage.data());
					const std::string tokenAuth = binary->token_auth()->str();
					const std::string userName = binary->user_name()->str();
					const auto &[isAwaited, playerAwaitedIt] = isPlayerAwaited(userName, tokenAuth, binary->fighting_pit_id());
					unsigned fightingPitId = playerAwaitedIt->second.fightingPitId;

					if (!isAwaited) {
						spdlog::warn("Player {} tried to authenticate on Arena server {} fighting pit {} "
									 "without being awaited.", userName, _ctx.get().getServerCode(), fightingPitId);
						return;
					}

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
					spdlog::info("Awaited player {} has logged in fighting pit of id:{}", userName, fightingPitId);
					_workerService.upsertPlayerIdentifier(fightingPitId, idtPlayer.str(), userName);
					_workerService.sendMsgNewArrivingTeam(fightingPitId, userName);
				},

				// InGame handler: Player is sending actions to feed pendingActions queue of their characters
				[this](zmq::message_t&& idtPlayer, const zmq::message_t& intermediate, zmq::message_t&& playerMsg) {
					const auto authFrame = fys::fb::GetArenaServerValidateAuth(intermediate.data());
					const std::string userName = authFrame->user_name()->str();
					auto fp = _workerService.getAuthenticatedPlayerFightingPit(userName,
							authFrame->token_auth()->str(),
							authFrame->fighting_pit_id());

					if (!fp) {
						spdlog::warn("Player {}:{} is not authenticated.", userName, authFrame->token_auth()->str());
						return;
					}
					_workerService.upsertPlayerIdentifier(authFrame->fighting_pit_id(), userName, idtPlayer.str());

					unsigned idMember = 0;
					// todo create an action message to forward
					// todo store into action: action id, target
					spdlog::debug("InGame Message received : {}", playerMsg.to_string());
					// if action is set to "IAMREADY", target id 1337
					fp->get().setPlayerReadiness(userName);

					fp->get().forwardMessageToTeamMember(userName, PlayerAction{});
				});
	}
	t.join();
}

bool
ArenaServerService::isSaturated() const noexcept
{
	const unsigned awaitedInstance = _awaitingArena.size();
	return (awaitedInstance + _workerService.getNumberBattleRunning()) > _ctx.get().getBattleThreshold();
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

	SPDLOG_INFO("New fighting pit is created name {}, token {} lvl {} "
				"encounter_id {} isAmbush {} wsCode {} isJoinDisabled {}",
			awaited.namePlayer, awaited.token, awaited.gen->levelFightingPit, awaited.gen->encounterId,
			awaited.gen->isAmbush, awaited.gen->serverCode, awaited.gen->isJoinDisabled);

	return id;
}

} // namespace fys::arena