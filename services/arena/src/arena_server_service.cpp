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

#include <network/db_connector.hh>
#include <fightingPit/fighting_pit_announcer.hh>

#include <arena_server_context.hh>
#include <flatbuffer_generator.hh>
#include <history_manager.hh>
#include "arena_server_service.hh"

// anonymous namespace used for utility function to extract data from flatbuffer
namespace {

template<typename T>
[[nodiscard]] bool
verify_buffer(const void* fbBuffer, uint size)
{
	auto v = flatbuffers::Verifier(static_cast<const uint8_t*>(fbBuffer), size);
	return v.VerifyBuffer<T>();
}

[[nodiscard]] fys::arena::fighting_pit::level
translate_level_from_flatbuffer(const fys::fb::arn::Level& level)
{
	switch (level) {
		case fys::fb::arn::Level_EASY:return fys::arena::fighting_pit::level::EASY;
		case fys::fb::arn::Level_MEDIUM:return fys::arena::fighting_pit::level::MEDIUM;
		case fys::fb::arn::Level_HARD:return fys::arena::fighting_pit::level::HARD;
		default:return fys::arena::fighting_pit::level::EASY;
	}
}

[[nodiscard]] fys::arena::awaiting_player_arena
create_awaiting_player(const fys::fb::arn::FightingPitEncounter* binary)
{
	std::optional<fys::arena::awaiting_arena> awaitingArena = std::nullopt;

	// if fighting_pit_id is 0, a new arena has to be generated, in which case more data are extracted
	if (!binary->fighting_pit_id()) {
		awaitingArena = fys::arena::awaiting_arena{
				binary->world_server_id()->str(),
				binary->disable_join(),
				binary->is_ambush(),
				binary->id_encounter(),
				translate_level_from_flatbuffer(binary->level_encounter())
		};
	}
	return fys::arena::awaiting_player_arena{
			binary->user_name()->str(),
			binary->token_auth()->str(),
			binary->fighting_pit_id(),
			std::move(awaitingArena)
	};
}

}

namespace fys::arena {

arena_server_service::arena_server_service(const arena_server_context& ctx)
		:
		_ctx(ctx),
		_cache(ctx.getPathLocalStorageCache(), ctx.get_path_source_cache()),
		_dbConnector(std::make_unique<network::db_connector>(ctx))
{
	_connectionHandler.setupConnectionManager(ctx.get_dispatcher_connection_str());
	_workerService.setup_connection_manager(ctx);
}

void
arena_server_service::runServerLoop() noexcept
{
	SPDLOG_INFO("ArenaServer loop started");

	auto t = std::thread([this] { _workerService.start_fighting_pits_loop(); });
	while (true) {
		_connectionHandler.pollAndProcessMessageFromDispatcher(
				// World Server inform the Arena that a new awaited player is coming (create or join an existing fighting pit)
				[this](zmq::message_t&& identityWs, zmq::message_t&& worldServerMessage) {

					// In case of a saturation of the server, return an error to the dispatcher
					if (isSaturated()) {
						sendSaturatedErrorMsg(std::move(identityWs));
						return;
					}

					if (!verify_buffer<fb::arn::FightingPitEncounter>(worldServerMessage.data(), worldServerMessage.size())) {
						SPDLOG_ERROR("Wrongly formatted FightingPitEncounter buffer");
						return;
					}
					const auto* binary = fb::arn::GetFightingPitEncounter(worldServerMessage.data());

					awaiting_player_arena apa = create_awaiting_player(binary);

					if (auto[exist, joinable] = _workerService.fighting_pit_exist_and_joinable(apa.fighting_pit_id);
							!apa.has_to_be_generated() && !exist && !joinable) {
						SPDLOG_ERROR("Player {} can't be awaited to register on non-existing({})/joinable({})"
									 " fighting pit {}", exist, joinable, apa.name_player, apa.fighting_pit_id);
						return;
					}

					// register player incoming into arena instance with token given by worldserver used as key
					const auto &[elem, isRegistered] = _awaitingArena.insert({binary->token_auth()->str(), std::move(apa)});

					if (isRegistered) {
						forwardReplyToDispatcher(std::move(identityWs), elem->second);
						SPDLOG_INFO("New awaited player is added");
					}
				});

		_workerService.poll_and_process_msg_from_player(
				// Authentication handler : Player try to create/join a fighting pit. The player has to be awaited
				[this](zmq::message_t&& idtPlayer, zmq::message_t&& authMessage) {
					if (!verify_buffer<fb::ArenaServerValidateAuth>(authMessage.data(), authMessage.size())) {
						SPDLOG_ERROR("Wrongly formatted ArenaServerValidateAuth buffer");
						return;
					}

					const auto* binary = fb::arn::GetArenaServerValidateAuth(authMessage.data());
					const std::string tokenAuth = binary->token_auth()->str();
					const std::string userName = binary->user_name()->str();
					const auto &[isAwaited, playerAwaitedIt] = isPlayerAwaited(userName, tokenAuth, binary->fighting_pit_id());
					unsigned fightingPitId = playerAwaitedIt->second.fighting_pit_id;

					if (!isAwaited) {
						SPDLOG_WARN("Player '{}' tried to authenticate on Arena server '{}' fighting pit {} "
									"without being awaited.", userName, _ctx.get().get_server_code(), fightingPitId);
						return;
					}
					SPDLOG_INFO("Awaited Player '{}' login", userName);

					if (playerAwaitedIt->second.has_to_be_generated()) {
						fightingPitId = createNewFightingPit(playerAwaitedIt->second);
						_awaitingArena.erase(playerAwaitedIt); // remove player from awaited player
						if (fightingPitId == fighting_pit::CREATION_ERROR) return;
					}
					else {
						if (auto[exist, isJoinable] = _workerService.fighting_pit_exist_and_joinable(binary->fighting_pit_id());
								!exist || !isJoinable) {
							SPDLOG_ERROR("Player {} can't be join on non-existing({})/joinable({}) fighting pit {}",
									exist, isJoinable, userName, binary->fighting_pit_id());
							return;
						}

						auto pt = _dbConnector->retrieve_party_team(userName);
						_workerService.player_join_fighting_pit(fightingPitId, std::move(pt), _cache);
						_awaitingArena.erase(playerAwaitedIt); // remove player from awaited player
					}
					SPDLOG_INFO("[fp:{}] : Player '{}' has logged in", fightingPitId, userName);
					_workerService.upsert_player_identifier(fightingPitId, userName, idtPlayer.to_string());
					_workerService.send_msg_new_arriving_team(fightingPitId, userName);
				},

				// InGame handler: Player is sending actions to feed pendingActions queue of their characters
				[this](zmq::message_t&& idtPlayer, const zmq::message_t& intermediate, zmq::message_t&& playerMsg) {
					if (!verify_buffer<fb::ArenaServerValidateAuth>(intermediate.data(), intermediate.size())) {
						SPDLOG_ERROR("Wrongly formatted ArenaServerValidateAuth (auth frame) buffer");
						return;
					}
					if (!verify_buffer<fb::ArenaFightAction>(playerMsg.data(), playerMsg.size())) {
						SPDLOG_ERROR("Wrongly formatted ArenaFightAction buffer");
						return;
					}

					const auto* authFrame = fb::arn::GetArenaServerValidateAuth(intermediate.data());
					const std::string userName = authFrame->user_name()->str();
					auto fp = _workerService.get_authenticated_player_fighting_pit(userName,
							authFrame->token_auth()->str(),
							authFrame->fighting_pit_id());

					if (!fp) {
						SPDLOG_WARN("Player {}:{} is not authenticated.", userName, authFrame->token_auth()->str());
						return;
					}
					_workerService.upsert_player_identifier(authFrame->fighting_pit_id(), userName, idtPlayer.str());

					const auto* frame = fb::arn::GetArenaFightAction(playerMsg.data());
					std::string action = frame->actionId()->str();

					if (frame->memberId() == READY_ACTION_ID && action == READY_ACTION) {
						fp->get().set_player_readiness(userName);
					}
					else if (fp->get().is_battle_on_going()) {
						fp->get().forward_to_team_member(userName, createPlayerAction(std::move(action), frame));
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
arena_server_service::createPlayerAction(std::string&& action, const fb::arn::ArenaFightAction* frame) const
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
arena_server_service::isSaturated() const noexcept
{
	const unsigned numberArenaManagedByServer = _awaitingArena.size() + _workerService.get_number_battle_running();
	return numberArenaManagedByServer > _ctx.get().get_battle_threshold();
}

std::pair<bool, const std::unordered_map<std::string, awaiting_player_arena>::const_iterator>
arena_server_service::isPlayerAwaited(const std::string& name, const std::string& token, unsigned idFightingPit) const noexcept
{
	if (const auto it = _awaitingArena.find(token);
			it != _awaitingArena.end() && it->second.name_player == name && it->second.fighting_pit_id == idFightingPit)
		return {true, it};
	return {false, _awaitingArena.cend()};
}

void
arena_server_service::forwardReplyToDispatcher(zmq::message_t&& idtWs, const fys::arena::awaiting_player_arena& awaitArena) noexcept
{
	flatbuffers::FlatBufferBuilder fbb;
	auto asaFb = fb::arn::CreateArenaServerAuth(
			fbb,
			fbb.CreateString(awaitArena.name_player),
			fbb.CreateString(awaitArena.token),
			fbb.CreateString(_ctx.get().get_hostname()),
			fbb.CreateString(_ctx.get().get_connection_str()),
			_ctx.get().get_port(),
			awaitArena.gen ? fbb.CreateString(awaitArena.gen->serverCode) : fbb.CreateString(""));
	fb::arn::FinishArenaServerAuthBuffer(fbb, asaFb);
	zmq::multipart_t msg;
	msg.add(std::move(idtWs));
	msg.addmem(fbb.GetBufferPointer(), fbb.GetSize());
	_connectionHandler.sendMessageToDispatcher(std::move(msg));
}

unsigned
arena_server_service::createNewFightingPit(const awaiting_player_arena& awaited) noexcept
{
	fighting_pit_announcer fpa(_cache);

	fpa.set_difficulty(awaited.gen->levelFightingPit);
	fpa.set_encounter_id(awaited.gen->encounterId);
	fpa.enforce_ambush(awaited.gen->isAmbush);
	fpa.set_creator_user_name(awaited.name_player);
	fpa.set_creator_user_token(awaited.token);
	fpa.set_creator_team_party(_dbConnector->retrieve_party_team(awaited.name_player));
	fpa.set_join_disabled(awaited.gen->isJoinDisabled);

	unsigned id = _workerService.add_fighting_pit(
			fpa.build_fighting_pit(_ctx.get().get_encounter_context(), awaited.gen->serverCode));

	if (id != fighting_pit::CREATION_ERROR) {
		SPDLOG_INFO("[fp:{}] : Player '{}' create new FightingPit : token '{}', lvl '{}', encounter_id '{}', isAmbush '{}', "
					"wsCode '{}', isJoinDisabled '{}'",
				id, awaited.name_player, awaited.token, awaited.gen->levelFightingPit, awaited.gen->encounterId,
				awaited.gen->isAmbush, awaited.gen->serverCode, awaited.gen->isJoinDisabled);
	}
	return id;
}

void
arena_server_service::sendSaturatedErrorMsg(zmq::message_t&& identity)
{
	SPDLOG_WARN("Arena Server Service is saturated and thus can't process new incoming fighting pit");
	auto[error, size] = fys::arena::flatbuffer_generator().generateErrorSaturated(_ctx.get().get_server_code());
	_workerService.direct_send_msg_to_player(std::move(identity), zmq::message_t(error, size));
}

} // namespace fys::arena