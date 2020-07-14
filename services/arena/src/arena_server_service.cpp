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

#include <fightingPit/team/party_team.hh>
#include <fightingPit/fighting_pit_announcer.hh>

#include <arena_server_context.hh>
#include <flatbuffer_generator.hh>
#include <history_manager.hh>
#include "arena_server_service.hh"

// anonymous namespace used for utility function to extract data from flatbuffer
namespace {

template<typename T>
[[nodiscard]] bool
verify_buffer(const void* fbBuffer, std::uint32_t size)
{
	auto v = flatbuffers::Verifier(static_cast<const uint8_t*>(fbBuffer), size);
	return v.VerifyBuffer<T>();
}

[[nodiscard]] fys::arena::fighting_pit::level
translate_level_from_flatbuffer(const fys::fb::arn::Level& level)
{
	switch (level) {
		case fys::fb::arn::Level_EASY:
			return fys::arena::fighting_pit::level::EASY;
		case fys::fb::arn::Level_MEDIUM:
			return fys::arena::fighting_pit::level::MEDIUM;
		case fys::fb::arn::Level_HARD:
			return fys::arena::fighting_pit::level::HARD;
		default:
			return fys::arena::fighting_pit::level::EASY;
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

arena_server_service::~arena_server_service() = default;

arena_server_service::arena_server_service(const arena_server_context& ctx)
		:
		_ctx(ctx),
		_cache(ctx.path_local_storage_cache(), ctx.path_source_cache()),
		_db_connector(std::make_unique<network::db_connector>(ctx))
{
	_connection_handler.setup_connection_manager(ctx.dispatcher_connection_str());
	_worker_service.setup_connection_manager(ctx);
}

void
arena_server_service::run_server_loop() noexcept
{
	SPDLOG_INFO("ArenaServer loop started");

	auto t = std::thread([this] { _worker_service.start_fighting_pits_loop(); });
	while (true) {
		_connection_handler.poll_process_msg_from_dispatcher(
				// World Server inform the Arena that a new awaited player is coming (create or join an existing fighting pit)
				[this](zmq::message_t&& identity_ws, zmq::message_t&& world_server_msg) {

					// In case of a saturation of the server, return an error to the dispatcher
					if (is_saturated()) {
						send_saturated_error_msg(std::move(identity_ws));
						return;
					}

					if (!verify_buffer<fb::arn::FightingPitEncounter>(world_server_msg.data(), world_server_msg.size())) {
						SPDLOG_ERROR("Wrongly formatted FightingPitEncounter buffer");
						return;
					}
					const auto* binary = fb::arn::GetFightingPitEncounter(world_server_msg.data());

					awaiting_player_arena apa = create_awaiting_player(binary);

					if (auto[exist, joinable] = _worker_service.fighting_pit_exist_and_joinable(apa.fighting_pit_id);
							!apa.has_to_be_generated() && !exist && !joinable) {
						SPDLOG_ERROR("Player {} can't be awaited to register on non-existing({})/joinable({})"
									 " fighting pit {}", exist, joinable, apa.name_player, apa.fighting_pit_id);
						return;
					}

					// register player incoming into arena instance with token given by worldserver used as key
					const auto &[elem, is_registered] = _awaiting_arena.insert({binary->token_auth()->str(), std::move(apa)});

					if (is_registered) {
						forward_reply_to_dispatcher(std::move(identity_ws), elem->second);
						SPDLOG_INFO("New awaited player is added");
					}
				});

		_worker_service.poll_process_msg_from_player(
				// Authentication handler : Player try to create/join a fighting pit. The player has to be awaited
				[this](zmq::message_t&& idt_player, zmq::message_t&& auth_msg) {
					if (!verify_buffer<fb::arn::ArenaServerValidateAuth>(auth_msg.data(), auth_msg.size())) {
						SPDLOG_ERROR("Wrongly formatted ArenaServerValidateAuth buffer");
						return;
					}

					const auto* binary = fb::arn::GetArenaServerValidateAuth(auth_msg.data());
					const std::string token_auth = binary->token_auth()->str();
					const std::string user_name = binary->user_name()->str();
					const auto &[awaited, player_awaited_it] = is_player_awaited(user_name, token_auth, binary->fighting_pit_id());
					unsigned fp_id = player_awaited_it->second.fighting_pit_id;

					if (!awaited) {
						SPDLOG_WARN("Player '{}' tried to authenticate on Arena server '{}' fighting pit {} "
									"without being awaited.", user_name, _ctx.get().server_code(), fp_id);
						return;
					}
					SPDLOG_INFO("Awaited Player '{}' login", user_name);

					if (player_awaited_it->second.has_to_be_generated()) {
						fp_id = create_new_fighting_pit(player_awaited_it->second);
						_awaiting_arena.erase(player_awaited_it); // remove player from awaited player
						if (fp_id == fighting_pit::CREATION_ERROR) return;
					}
					else {
						if (auto[exist, is_joinable] = _worker_service.fighting_pit_exist_and_joinable(binary->fighting_pit_id());
								!exist || !is_joinable) {
							SPDLOG_ERROR("Player {} can't be join on non-existing({})/joinable({}) fighting pit {}",
									exist, is_joinable, user_name, binary->fighting_pit_id());
							return;
						}

						auto pt = _db_connector->retrieve_party_team(user_name);
						_worker_service.player_join_fighting_pit(fp_id, std::move(pt), _cache);
						_awaiting_arena.erase(player_awaited_it); // remove player from awaited player
					}
					SPDLOG_INFO("[fp:{}] : Player '{}' has logged in", fp_id, user_name);
					_worker_service.upsert_player_identifier(fp_id, user_name, idt_player.to_string());
					_worker_service.send_msg_new_arriving_team(fp_id, user_name);
				},

				// InGame handler: Player is sending actions to feed pendingActions queue of their characters
				[this](zmq::message_t&& idt_player, const zmq::message_t& intermediate, zmq::message_t&& playerMsg) {
					if (!verify_buffer<fb::arn::ArenaServerValidateAuth>(intermediate.data(), intermediate.size())) {
						SPDLOG_ERROR("Wrongly formatted ArenaServerValidateAuth (auth frame) buffer");
						return;
					}
					if (!verify_buffer<fb::arn::ArenaFightAction>(playerMsg.data(), playerMsg.size())) {
						SPDLOG_ERROR("Wrongly formatted ArenaFightAction buffer");
						return;
					}

					const auto* auth_frame = fb::arn::GetArenaServerValidateAuth(intermediate.data());
					const std::string user_name = auth_frame->user_name()->str();
					auto fp = _worker_service.get_authenticated_player_fighting_pit(user_name,
							auth_frame->token_auth()->str(),
							auth_frame->fighting_pit_id());

					if (!fp) {
						SPDLOG_WARN("Player {}:{} is not authenticated.", user_name, auth_frame->token_auth()->str());
						return;
					}
					_worker_service.upsert_player_identifier(auth_frame->fighting_pit_id(), user_name, idt_player.str());

					const auto* frame = fb::arn::GetArenaFightAction(playerMsg.data());
					std::string action = frame->actionId()->str();

					if (frame->memberId() == READY_ACTION_ID && action == READY_ACTION) {
						fp->get().set_player_readiness(user_name);
					}
					else if (fp->get().is_battle_on_going()) {
						fp->get().forward_to_team_member(user_name, create_player_action(std::move(action), frame));
					}
					else {
						SPDLOG_WARN("An action {} action has been received from player {} "
									"while the battle isn't on-going", action, user_name);
					}
				});
	}
	t.join();
}

player_action
arena_server_service::create_player_action(std::string&& action, const fb::arn::ArenaFightAction* frame) const
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
	return player_action{
			frame->memberId(),
			std::move(action),
			std::move(contenders),
			std::move(allies)};
}

bool
arena_server_service::is_saturated() const noexcept
{
	const unsigned number_arena_managed_by_server = _awaiting_arena.size() + _worker_service.number_battle_running();
	return number_arena_managed_by_server > _ctx.get().battle_threshold();
}

std::pair<bool, const std::unordered_map<std::string, awaiting_player_arena>::const_iterator>
arena_server_service::is_player_awaited(const std::string& name, const std::string& token, unsigned fp_id) const noexcept
{
	if (const auto it = _awaiting_arena.find(token);
			it != _awaiting_arena.end() && it->second.name_player == name && it->second.fighting_pit_id == fp_id)
		return {true, it};
	return {false, _awaiting_arena.cend()};
}

void
arena_server_service::forward_reply_to_dispatcher(zmq::message_t&& idt_ws, const fys::arena::awaiting_player_arena& await_arena) noexcept
{
	flatbuffers::FlatBufferBuilder fbb;
	auto asaFb = fb::arn::CreateArenaServerAuth(
			fbb,
			fbb.CreateString(await_arena.name_player),
			fbb.CreateString(await_arena.token),
			fbb.CreateString(_ctx.get().hostname()),
			fbb.CreateString(_ctx.get().connection_str()),
			_ctx.get().port(),
			await_arena.gen ? fbb.CreateString(await_arena.gen->server_code) : fbb.CreateString(""));
	fb::arn::FinishArenaServerAuthBuffer(fbb, asaFb);
	zmq::multipart_t msg;
	msg.add(std::move(idt_ws));
	msg.addmem(fbb.GetBufferPointer(), fbb.GetSize());
	_connection_handler.send_msg_to_dispatcher(std::move(msg));
}

unsigned
arena_server_service::create_new_fighting_pit(const awaiting_player_arena& awaited) noexcept
{
	fighting_pit_announcer fpa(_cache);

	fpa.set_difficulty(awaited.gen->level_fighting_pit);
	fpa.set_encounter_id(awaited.gen->encounter_id);
	fpa.enforce_ambush(awaited.gen->is_ambush);
	fpa.set_creator_user_name(awaited.name_player);
	fpa.set_creator_user_token(awaited.token);
	fpa.set_creator_team_party(_db_connector->retrieve_party_team(awaited.name_player));
	fpa.set_join_disabled(awaited.gen->is_join_disabled);

	unsigned id = _worker_service.add_fighting_pit(
			fpa.build_fighting_pit(_ctx.get().encounter_ctx(), awaited.gen->server_code));

	if (id != fighting_pit::CREATION_ERROR) {
		SPDLOG_INFO("[fp:{}] : Player '{}' create new FightingPit : token '{}', lvl '{}', encounter_id '{}', isAmbush '{}', "
					"wsCode '{}', isJoinDisabled '{}'",
				id, awaited.name_player, awaited.token, awaited.gen->level_fighting_pit, awaited.gen->encounter_id,
				awaited.gen->is_ambush, awaited.gen->server_code, awaited.gen->is_join_disabled);
	}
	return id;
}

void
arena_server_service::send_saturated_error_msg(zmq::message_t&& identity)
{
	SPDLOG_WARN("Arena Server Service is saturated and thus can't process new incoming fighting pit");
	auto[error, size] = fys::arena::flatbuffer_generator().generateErrorSaturated(_ctx.get().server_code());
	_worker_service.direct_send_msg_to_player(std::move(identity), zmq::message_t(error, size));
}

} // namespace fys::arena