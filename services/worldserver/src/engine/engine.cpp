// MIT License
//
// Copyright (c) 2021-2022 Quentin Balland
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

#include <Notifications_generated.h>
#include <WSAction_generated.h>
#include <connection_handler.hh>

#include <world_server_context.hh>

#include <engine/engine.hh>
#include <engine/script_engine.hh>
#include <flatbuffer_generator.hh>

namespace fys::ws {

[[nodiscard]] inline pos
calculate_potential_future_position(const character_info& info) noexcept {
  return pos{
      info.position.x * (info.velocity * std::cos(info.angle)),
      info.position.y * (info.velocity * std::sin(info.angle))};
}

struct engine::internal {

  internal(collision_map&& map, std::chrono::system_clock::duration time_interval)
      : map(std::move(map)), next_tick(std::chrono::system_clock::now() + time_interval) {}

  collision_map map;
  player_data data;

  //! Next movement tick should take place at this moment
  std::chrono::system_clock::time_point next_tick;
};

engine::engine(const std::string& player_connect_str,
               collision_map&& map,
               std::unique_ptr<script_engine> engine,
               std::chrono::system_clock::duration time_interval)
    : common::direct_connection_manager(1, player_connect_str),
      _intern(std::make_unique<internal>(std::move(map), time_interval)),
      _script_engine(std::move(engine)) {}

engine::~engine() = default;
engine::engine(engine&&) noexcept = default;

void engine::authenticate_player(auth_player auth, character_info info, std::string identifier) {
  std::uint32_t index = _intern->data.add_new_player_data(std::move(info), std::move(identifier), auth.user_name);
  _auth_player_on_data_index.insert(std::pair(std::move(auth), index));
}

void engine::set_player_move_direction(std::uint32_t index, double direction) {
  _intern->data.set_player_move_action(index, direction);
}

void engine::stop_player_move(std::uint32_t index) {
  _intern->data.stop_player_move(index);
}

void engine::execute_pending_moves(const std::chrono::system_clock::time_point& current_time) {
  // Don't process pending moves if it isn't ticking
  if (current_time < _intern->next_tick) {
    return;
  }
  // Setup next tick time point
  _intern->next_tick += TIMING_MOVE_INTERVAL;

  // execute and report players movements
  _intern->data.execution_on_player(
      [this](std::uint32_t playerIndex, player_status status, character_info& info, const std::string&, const std::string& userName) {
        if (status == player_status::MOVING) {
          move_character_action(userName, playerIndex, info);
        }
      });

  // execute and report npc movements
  notify_reported_npc_movements(_script_engine->execute_scripted_actions());
}

void engine::move_character_action(const std::string& character_name, std::uint32_t index_character, character_info& info) {
  pos future_position = calculate_potential_future_position(info);

  if (_intern->map.can_move_to(future_position)) {
    info.position = future_position;

    _intern->map.execute_potential_trigger(future_position, index_character, info);

    if (auto idts_to_notify = _intern->data.get_player_idts_around_pos(info.position); !idts_to_notify.empty()) {
      notify_clients_of_character_move(info, character_name, idts_to_notify);
    }
  }
}

static constexpr unsigned index_identity = 0;
static constexpr unsigned index_notification = 1;

void engine::notify_reported_npc_movements(const npc_actions_report& action_report) {
  zmq::multipart_t to_send;

  to_send.add({});
  to_send.add({});

  for (std::uint32_t i = 0; i < action_report.npc_actions.size(); ++i) {

    if (action_report.npc_actions.at(i).empty()) {
      continue;
    }

    // get the identities to notify around the spawning point
    const auto idts_to_notify = _intern->data.get_player_idts_around_pos(action_report.central_positions.at(i));
    if (idts_to_notify.empty()) {
      continue;
    }

    auto [binary, size] = flatbuffer_generator().generate_bulk_move_notif(action_report.npc_actions.at(i));
    to_send.at(index_notification).rebuild(binary, size);

    for (const auto& identity : idts_to_notify) {
      to_send.at(index_identity).rebuild(identity.data(), identity.size());
      to_send.send(_router_player_connection);
    }
  }
}

void engine::notify_clients_of_character_move(
    const character_info& info,
    const std::string& user_name,
    const std::vector<std::string_view>& idts_to_identify) {
  zmq::multipart_t to_send;

  auto [binary, size] = flatbuffer_generator().generate_move_notif(user_name, info);
  to_send.add({});
  to_send.addmem(binary, size);
  for (const auto& identity : idts_to_identify) {
    to_send.at(index_identity).rebuild(identity.data(), identity.size());
    to_send.send(_router_player_connection);
  }
}

std::uint32_t engine::retrieve_data_index(const auth_player& player) noexcept {
  auto node = _auth_player_on_data_index.extract(player);
  if (node.empty()) {
    return NOT_AUTHENTICATED;
  }
  if (!node.key().is_responsible) {
    SPDLOG_ERROR("Trying to retrieve data_index for user {} while not being responsible of him.", player.user_name);
    return NOT_AUTHENTICATED;
  }

  std::uint32_t index = node.mapped();
  _auth_player_on_data_index.insert(std::move(node));
  return index;
}

bool engine::server_transition_update(const auth_player& transition_update) {
  auto ex = _auth_player_on_data_index.extract(transition_update);
  if (ex.empty()) {
    return false;
  }
  if (ex.key().is_responsible) {
    SPDLOG_ERROR("Player {} is already under responsibility. Cannot do a server player transition", transition_update.user_name);
    return false;
  }

  ex.key() = transition_update;
  _auth_player_on_data_index.insert(std::move(ex));
  return true;
}

}// namespace fys::ws
