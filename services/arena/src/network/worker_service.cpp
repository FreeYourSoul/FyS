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

#include <chrono>
#include <thread>

#include <chaiscript/chaiscript.hpp>
#include <fmt/format.h>

#include <FightingPitState_generated.h>

#include <fightingPit/fighting_pit.hh>
#include <fightingPit/team/party_team.hh>

#include <arena_server_context.hh>
#include <flatbuffer_generator.hh>
#include <history_manager.hh>
#include <logger.hh>
#include <network/worker_service.hh>

namespace fys::arena {

worker_service::worker_service()
    : _ctx(1), _worker_router(_ctx, zmq::socket_type::router), _current_arena_id(0) {}

void worker_service::setup_connection_manager(const fys::arena::arena_server_context& ctx) noexcept {
  _worker_router.bind(ctx.player_binding_string());
}

void worker_service::start_fighting_pits_loop() {
  log_info("WorkerService FightingPit game loops started");

  using namespace std::chrono_literals;
  while (true) {
    auto now = std::chrono::system_clock::now();

    if (!_arena_instances.empty()) {
      for (auto& [id, fp] : _arena_instances) {
        if (fp->is_battle_on_going()) {
          fp->continue_battle(now);
        } else if (!fp->is_joinable()) {
          fp->notify_end_status(broadcast_msg_handler(id));
        }
      }
      cleanup_finished_battles();
    }
    std::this_thread::sleep_for(100ms);
  }
}

unsigned
worker_service::add_fighting_pit(std::unique_ptr<fighting_pit> fp) {
  if (!fp) {
    log_error("Cannot add fighting pit in WorkerService");
    return fighting_pit::CREATION_ERROR;
  }
  if (_arena_instances.size() >= SERVER_FULL_CAPACITY) {
    log_error("Cannot add fighting pit in WorkerService (worker full)");
    return fighting_pit::CREATION_ERROR;
  }

  while (++_current_arena_id != 0 && _arena_instances.find(_current_arena_id) != _arena_instances.end())
    ;
  chai_register::register_network_commands(*fp->get_chai_ptr(), broadcast_msg_handler(fp->id()));
  fp->set_arena_id(_current_arena_id);
  history_manager::create_historic(*fp, 1);// seed is to be changed
  _arena_instances.insert(std::pair(_current_arena_id, std::move(fp)));
  _arena_id_on_identifier.insert(std::pair(_current_arena_id, std::vector<player_identifier>{}));
  return _current_arena_id;
}

void worker_service::cleanup_finished_battles() {
  std::vector<unsigned> idsToRemove;
  idsToRemove.reserve(_arena_instances.size());
  for (auto& [id, fp] : _arena_instances) {
    if (fp->is_battle_over()) {
      idsToRemove.emplace_back(id);
    }
  }
  for (unsigned id : idsToRemove) {
    history_manager::save(id);
    _arena_instances.erase(id);
    _arena_id_on_identifier.erase(id);
  }
}

void worker_service::player_join_fighting_pit(unsigned fighting_pit_id, std::unique_ptr<party_team> pt, cache::cml& cml) {
  auto it = _arena_instances.find(fighting_pit_id);
  if (it != _arena_instances.end()) {
    if (it->second->is_joinable()) {
      it->second->add_party_team_and_register_actions(std::move(pt), cml);
    } else {
      log_info(fmt::format("User {} can't join fighting pit of id {} as the battle already started.",
                           pt->user_name(), fighting_pit_id));
    }
  } else {
    log_error(fmt::format("PartyTeam of user {} can't join fighting pit of id {}",
                          pt->user_name(), fighting_pit_id));
  }
}

std::optional<std::reference_wrapper<fighting_pit>>
worker_service::get_authenticated_player_fighting_pit(const std::string& name, const std::string& token, unsigned fp_id) {
  if (auto it = _arena_instances.find(fp_id);
      (it != _arena_instances.end() && it->second->is_player_participant(name, token))) {
    return *it->second;
  }
  log_warn(fmt::format("Request received from {}:{} for arena id {}, but arena isn't defined", name, token, fp_id));
  return std::nullopt;
}

void worker_service::upsert_player_identifier(unsigned fp_id, std::string user_name, std::string idt_player) {
  auto itIdt = _arena_id_on_identifier.find(fp_id);
  if (itIdt == _arena_id_on_identifier.end())
    return;

  auto& identifiers = itIdt->second;
  if (auto it = std::ranges::find_if(identifiers, [&user_name](const auto& idt) { return idt.user_name == user_name; });
      (it != identifiers.end())) {
    it->identifier = std::move(idt_player);
  } else {
    identifiers.emplace_back(player_identifier{std::move(user_name), std::move(idt_player)});
  }
}

std::pair<bool, bool>
worker_service::fighting_pit_exist_and_joinable(unsigned int fighting_pit_id) const noexcept {
  auto it = _arena_instances.find(fighting_pit_id);
  return std::pair(it != _arena_instances.cend(), it != _arena_instances.cend() && it->second->is_joinable());
}

void worker_service::send_msg_new_arriving_team(unsigned fp_id, const std::string& user_name) noexcept {
  flatbuffer_generator fg;

  {
    auto [fbMsg, size] = fg.generateFightingPitState(*_arena_instances.at(fp_id));
    send_msg_to_player(fp_id, user_name, zmq::message_t(fbMsg, size));
  }
  {
    auto [fbMsg, size] = fg.generate_party_team_status(_arena_instances.at(fp_id)->get_party_team_of_player(user_name));
    broadcast_msg(fp_id, zmq::message_t(fbMsg, size), user_name);
  }
}

void worker_service::direct_send_msg_to_player(zmq::message_t&& identity, zmq::message_t&& msg) {
  zmq::multipart_t toSend;
  toSend.add(std::move(identity));
  toSend.add(std::move(msg));
  if (!toSend.send(_worker_router)) {
    log_error(fmt::format("Failure to send directly the message: {}", toSend.str()));
  }
}

bool worker_service::send_msg_to_player(unsigned fp_id, const std::string& user_name, zmq::message_t&& msg) {
  const std::string& identifier = retrieve_player_identifier(fp_id, user_name);
  if (identifier == user_name) {
    log_error(fmt::format("Cannot send a message in fightingPit {} to player {}. {}", fp_id, user_name, msg.str()));
    return false;
  }
  zmq::multipart_t toSend;
  toSend.addstr(identifier);
  toSend.add(std::move(msg));
  log_debug(fmt::format("Send message to player {} with identifier {}", user_name, identifier));
  if (!toSend.send(_worker_router)) {
    log_error(fmt::format("Failure to send message to player {} in fightingPit {}.", user_name, fp_id));
    return false;
  }
  return true;
}

bool worker_service::broadcast_msg(unsigned fp_id, zmq::message_t&& msg, const std::string& except) {
  const auto identifiersIt = _arena_id_on_identifier.find(fp_id);
  if (identifiersIt == _arena_id_on_identifier.end())
    return false;
  zmq::multipart_t toSend;

  toSend.add({});            // add empty frame (identifier frame at index 0)
  toSend.add(std::move(msg));// add content

  for (const auto& [user_name, identifier] : identifiersIt->second) {
    if (user_name == except)
      continue;
    toSend.at(0).rebuild(identifier.data(), identifier.size());
    log_debug(fmt::format("Send message to player {} with identifier {}", user_name, identifier));
    if (!toSend.send(_worker_router)) {
      log_error(fmt::format("fightingPit of id {} : Message has not been correctly sent to {}, {}", fp_id, user_name, identifier));
      return false;
    }
  }
  return true;
}

const std::string&
worker_service::retrieve_player_identifier(unsigned fp_id, const std::string& user_name) {
  auto identifiers_it = _arena_id_on_identifier.find(fp_id);
  if (identifiers_it == _arena_id_on_identifier.end()) {
    log_warn(fmt::format("Trying to retrieve player identifier on non-existing fighting pit of id {}", fp_id));
    return user_name;
  }

  auto it = std::ranges::find_if(identifiers_it->second, [&user_name](const auto& ident) {
    return ident.user_name == user_name;
  });
  if (it == identifiers_it->second.end()) {
    log_warn(fmt::format("Trying to retrieve non-existing player identifier {} in fighting pit of id {}", user_name, fp_id));
    return user_name;
  }
  return it->identifier;
}

}// namespace fys::arena
