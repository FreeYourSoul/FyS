// MIT License
//
// Copyright (c) 2021 Quentin Balland
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

#include <utility>

#include <spdlog/spdlog.h>
#include <zmq_addon.hpp>

#include <AuthFrame_generated.h>
#include <IncomingPlayerOnWs_generated.h>
#include <InterServerCom_generated.h>
#include <Notifications_generated.h>
#include <WSAction_generated.h>
#include <flatbuffers/flatbuffers.h>

#include "world_server_service.hh"

namespace {
template<typename T>
[[nodiscard]] bool
verify_buffer(const void* fbBuffer, std::uint32_t size) {
  auto v = flatbuffers::Verifier(static_cast<const uint8_t*>(fbBuffer), size);
  return v.VerifyBuffer<T>();
}
}// namespace

namespace fys::ws {

world_server_service::world_server_service(const world_server_context& ctx, engine engine)
    : _ctx(ctx), _world_server(std::move(engine)) {
  _connection_handler.setup_connection_manager(ctx);
}

void world_server_service::run_server_loop() noexcept {
  SPDLOG_INFO("WorldServer loop started");

  while (true) {
    _connection_handler.poll_and_process_sub_msg(
        // Auth-Server Incoming player
        // Used by the Auth server in order to authenticate a player
        [this]([[maybe_unused]] zmq::message_t&& identity, zmq::message_t&& content) {
          if (verify_buffer<fb::auth::IncomingPlayerOnWs>(content.data(), content.size())) {
            SPDLOG_ERROR("Wrongly formatted IncomingPlayerOnWs buffer");
            return;
          }
          auto* frame = fb::auth::GetIncomingPlayerOnWs(content.data());

          _awaited_incoming_player.emplace_back(
              awaited_player{
                  auth_player{frame->userName()->str(), frame->token()->str()},
                  pos{frame->posX(), frame->posY()},
                  frame->angle(),
                  frame->velocity()});
        },

        // Inter-Server communication
        // Used when world server are communicating between each others to handle player transition
        [this]([[maybe_unused]] zmq::message_t&& identity, zmq::message_t&& content) {
          if (verify_buffer<fb::world::InterServerCom>(content.data(), content.size())) {
            SPDLOG_ERROR("Wrongly formatted InterServerCom buffer");
            return;
          }
          auto* server_com = fb::world::GetInterServerCom(content.data());

          switch (server_com->content_type()) {
          case fb::world::InterServerComContent_InterServerComMove:
            process_transition_server_movement(server_com->content_as_InterServerComMove());
            break;
          default: SPDLOG_ERROR("This inter server com isn't handled by WorldServer Service '{}'", server_com->content_type());
          }
        });

    _world_server.pollAndProcessPlayerMessage(

        // direct player communication
        // Used when a player is communicating an interaction with the world_server
        [this](zmq::message_t&& identity, zmq::message_t&& auth_frame_msg, zmq::message_t&& content) {
          if (!verify_buffer<fb::world::AuthFrame>(auth_frame_msg.data(), auth_frame_msg.size())) {
            SPDLOG_ERROR("Ill formatted Authentication frame");
            return;
          }
          if (!verify_buffer<fb::world::WSAction>(content.data(), content.size())) {
            SPDLOG_ERROR("Ill formatted Action frame");
            return;
          }
          auto* auth_frame = fb::world::GetAuthFrame(auth_frame_msg.data());
          auto* frame = fb::world::GetWSAction(content.data());

          if (frame->action_type() == fb::world::Action_ValidateAuth) {
            register_awaited_player(auth_frame->userName()->str(), auth_frame->token()->str(), identity.to_string());
          } else {
            process_player_message(auth_frame->userName()->str(), auth_frame->token()->str(), frame);
          }
        });

    auto now = std::chrono::system_clock::now();
    _world_server.spawnNPC(now);
    _world_server.execute_pending_moves(now);
  }
}

void world_server_service::process_player_message(const std::string& user, const std::string& tkn, const fb::world::WSAction* action) {
  std::uint32_t index = _world_server.retrieve_data_index({user, tkn});
  if (index == NOT_AUTHENTICATED) {
    SPDLOG_ERROR("Player '{}' isn't authenticated for movement", user);
    return;
  }

  switch (action->action_type()) {
  case fb::world::Action_StopMove:
    _world_server.stop_player_move(index);
    break;
  case fb::world::Action_Move:
    _world_server.set_player_move_direction(index, action->action_as_Move()->direction());
    break;
  case fb::world::Action_PnjInteract:
  case fb::world::Action_JoinArena:
    SPDLOG_INFO("NOT_IMPLEMENTED YET");
    break;
  default: SPDLOG_ERROR("This action isn't handled by WorldServer Service '{}'", action->action_type());
  }
  SPDLOG_DEBUG("message received with idt='{}', token='{}'", idt, tkn);
}

void world_server_service::process_transition_server_movement(const fb::world::InterServerComMove* move_transition) {
  auth_player transition_player{
      move_transition->user()->str(), move_transition->token()->str(), move_transition->giveResponsibility()};

  // update player with provided one if authenticated
  const bool update_worked = _world_server.server_transition_update(transition_player);

  if (!update_worked) {
    // if update didn't work, then upsert the player in the awaited ones

    auto awaited_it = std::ranges::find_if(_awaited_incoming_player, [&transition_player](const auto& awaited) {
      return awaited.auth == transition_player;
    });

    if (awaited_it != _awaited_incoming_player.end()) {
      // if already awaited, update awaited player with given one
      awaited_it->auth = std::move(transition_player);

    } else {
      // otherwise add it in the awaited list
      _awaited_incoming_player.emplace_back(
          awaited_player{
              std::move(transition_player),
              pos{move_transition->posX(), move_transition->posY()},
              move_transition->angle(),
              move_transition->velocity()});
    }
  }
}

void world_server_service::register_awaited_player(const std::string& user, const std::string& token, std::string identity) {
  auto awaited_it = std::ranges::find_if(_awaited_incoming_player, [toCheck = auth_player{user, token}](const auto& awaited) {
    return awaited.auth == toCheck;
  });
  if (awaited_it == _awaited_incoming_player.end()) {
    SPDLOG_ERROR("Player '{}' isn't awaited with the given token", user, token);
    return;
  }

  _world_server.authenticate_player(
      awaited_it->auth,
      character_info{awaited_it->initial_position, awaited_it->initial_velocity, awaited_it->initial_angle},
      std::move(identity));
  _awaited_incoming_player.erase(awaited_it);
}

}// namespace fys::ws