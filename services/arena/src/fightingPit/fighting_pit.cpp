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

#include <algorithm>
#include <spdlog/spdlog.h>

#include <chai_register.hh>
#include <util/ChaiUtility.hh>

#include <fil/algorithm/contains.hh>

#include <history_manager.hh>
#include <network/worker_service.hh>

#include <fightingPit/contender/fighting_contender.hh>
#include <fightingPit/fighting_pit.hh>
#include <fightingPit/rewards.hh>
#include <fightingPit/team/party_team.hh>
#include <fightingPit/team/team_member.hh>
#include <flatbuffer_generator.hh>

namespace {

[[nodiscard]] std::chrono::milliseconds
retrieve_time_interlude_from_level_degree(fys::arena::fighting_pit::level level) {
  switch (level) {
  case fys::arena::fighting_pit::level::EASY: return fys::arena::interval::EASY;
  case fys::arena::fighting_pit::level::MEDIUM: return fys::arena::interval::MEDIUM;
  case fys::arena::fighting_pit::level::HARD: return fys::arena::interval::HARD;
  default:
    SPDLOG_ERROR("Incorrect level");
    return std::chrono::milliseconds{0};
  }
}

}// namespace

namespace fys::arena {

fighting_pit::fighting_pit(std::string creator_user_name, fys::arena::fighting_pit::level level_fighting_pit)
    : _progress(progress::ON_HOLD),
      _level_fighting_pit(level_fighting_pit),
      _time_interlude(retrieve_time_interlude_from_level_degree(_level_fighting_pit)),
      _layout_map(_contenders, _party_teams),
      _creator_user_name(std::move(creator_user_name)),
      _chai_ptr(chai_register::make_chai_instance(_contenders, _party_teams, _layout_map)),
      _rewards(std::make_unique<rewards>()) {}

fighting_pit::~fighting_pit() = default;

void fighting_pit::continue_battle(const std::chrono::system_clock::time_point& now) {
  for (auto& battle : _side_battles) {
    if (battle.empty()) {// if battle side is empty, ignore it
      continue;
    }
    auto current_participant = battle.get_current_participant_turn(now, _time_interlude);
    bool turn_executed = false;

    if (current_participant.is_contender) {
      // non-playable character (enemy NPC)
      _contenders.execute_contender_action(current_participant);
      turn_executed = true;
    } else {
      // character of a player
      turn_executed = _party_teams.execute_ally_action(current_participant, _contenders, _chai_ptr);
    }
    battle.turn_done(turn_executed);
    battle.erase_finished_alteration_and_dead_characters_from_turn_list();
  }
  // execute awaited movements
  _layout_map.execute_movements(_side_battles);

  _progress = update_progress_status();
}

fighting_pit::progress
fighting_pit::update_progress_status() {
  if (_party_teams.all_dead()) {
    return progress::CONTENDER_WIN;
  } else if (_contenders.all_dead()) {
    history_manager::set_to_be_saved(_arena_id, false);
    return progress::ALLY_WIN;
  }
  return progress::ON_GOING;
}

void fighting_pit::forward_to_team_member(const std::string& user, player_action action) {
  auto member = _party_teams.get_specific_team_member_by_id(user, action.id_member);
  if (!member) {
    SPDLOG_ERROR("[fp:{}] : Player '{}' try to forward a message to not existing member member of id {}",
                 _arena_id, user, action.id_member);
    return;
  }
  try {
    if (!chai::util::member_has_action_registered(*_chai_ptr, user, member->name(), action.action_name)) {
      SPDLOG_WARN("[fp:{}] : Member {}.{}.{} tried to execute Action '{}' which isn't registered for member '{}'",
                  _arena_id, user, member->id(), member->name(), action.action_name, member->name());
      return;
    }
    auto [targetIsCorrect, target] = this->check_and_retrieve_target(user, member, action);
    if (targetIsCorrect) {
      SPDLOG_INFO("[fp:{}] : Member {}.{}.{} register a new action {}", _arena_id, user, member->id(), member->name(), action.action_name);
      member->add_pending_action(std::move(action.action_name), std::move(target));
    }
  } catch (const std::exception& e) {
    SPDLOG_ERROR("[fp:{}] : An error occurred when checking if an action was doable from user '{}':\n{}",
                 _arena_id, user, e.what());
  }
}

void fighting_pit::add_authenticated_user(std::string user_name, std::string user_token) {
  _authenticated_players.push_back({std::move(user_name), std::move(user_token)});
}

bool fighting_pit::is_player_participant(const std::string& name, const std::string& token) const noexcept {
  return std::ranges::any_of(_authenticated_players, [&name, &token](auto& authPlayer) {
    return authPlayer.name == name && authPlayer.token == token;
  });
}

void fighting_pit::add_party_team_and_register_actions(std::unique_ptr<party_team> pt, cache::Cml& cache) {
  chai_register::load_register_action_party_team(*_chai_ptr, cache, *pt);
  _layout_map.add_active_party_team(*pt);
  _party_teams.add_party_team(std::move(pt));
  std::sort(_side_battles.begin(), _side_battles.end(),
            [this](auto& lhs, auto& rhs) {
              return _layout_map.active_characters_on_side(lhs.side()) < _layout_map.active_characters_on_side(rhs.side());
            });
}

// Initialization methods used by FightingPitAnnouncer

void fighting_pit::initialize_side_battles() {
  _side_battles.reserve(hexagon_side::SIDE_NUMBER);

  // Hexagon A creation
  _side_battles.emplace_back(_contenders, _party_teams, hexagon_side::orientation::A_N);
  _side_battles.emplace_back(_contenders, _party_teams, hexagon_side::orientation::A_NE);
  _side_battles.emplace_back(_contenders, _party_teams, hexagon_side::orientation::A_SE);
  _side_battles.emplace_back(_contenders, _party_teams, hexagon_side::orientation::A_S);
  _side_battles.emplace_back(_contenders, _party_teams, hexagon_side::orientation::A_SW);
  _side_battles.emplace_back(_contenders, _party_teams, hexagon_side::orientation::A_NW);

  // Hexagon B creation
  _side_battles.emplace_back(_contenders, _party_teams, hexagon_side::orientation::B_N);
  _side_battles.emplace_back(_contenders, _party_teams, hexagon_side::orientation::B_NE);
  _side_battles.emplace_back(_contenders, _party_teams, hexagon_side::orientation::B_SE);
  _side_battles.emplace_back(_contenders, _party_teams, hexagon_side::orientation::B_S);
  _side_battles.emplace_back(_contenders, _party_teams, hexagon_side::orientation::B_SW);
  _side_battles.emplace_back(_contenders, _party_teams, hexagon_side::orientation::B_NW);

  // Hexagon C creation
  _side_battles.emplace_back(_contenders, _party_teams, hexagon_side::orientation::C_N);
  _side_battles.emplace_back(_contenders, _party_teams, hexagon_side::orientation::C_NE);
  _side_battles.emplace_back(_contenders, _party_teams, hexagon_side::orientation::C_SE);
  _side_battles.emplace_back(_contenders, _party_teams, hexagon_side::orientation::C_S);
  _side_battles.emplace_back(_contenders, _party_teams, hexagon_side::orientation::C_SW);
  _side_battles.emplace_back(_contenders, _party_teams, hexagon_side::orientation::C_NW);

  initialize_priority_in_side_battles();
}

void fighting_pit::initialize_priority_in_side_battles() {
  for (auto& sb : _side_battles) {
    const auto& member_by_side = _party_teams.members_by_side(sb.side());
    const auto& contender_by_side = _contenders.contenders_on_side(sb.side());

    for (const team_member_sptr& member : member_by_side) {
      sb.add_participant_in_list(member->id(), member->status().initial_speed, false);
    }
    for (unsigned i = 0; i < contender_by_side.size(); ++i) {
      sb.add_participant_in_list(i, contender_by_side.at(i)->status().initial_speed, true);
    }
  };
}
void fighting_pit::set_player_readiness(const std::string& user_name) {
  SPDLOG_INFO("[fp:{}] : Player '{}' is setting readiness", _arena_id, user_name);
  if (_party_teams.set_party_readiness(user_name)) {
    _progress = progress::ON_GOING;
  }
}
std::pair<bool, std::optional<target_type>>
fighting_pit::check_and_retrieve_target(const std::string& user, const team_member_sptr& member, const player_action& action) {
  auto t = _chai_ptr->eval<data::targeting>(
      chai::util::get_ally_action_retriever(user, member->name(), action.action_name).append(".requireTarget();"));
  std::optional<target_type> target;

  // Check if the target is appropriate
  if (t == data::targeting::SELF) {
    if (!action.ally_target.empty() || !action.contender_target.empty()) {
      SPDLOG_WARN("[fp:{}] : Action {} target type is SELF, but Player {} tried to execute it with targets set",
                  action.action_name, user);
      return std::pair(false, target);
    }
    target = std::nullopt;
  } else if (t == data::targeting::ENNEMY) {
    if (!action.ally_target.empty() || action.contender_target.empty()) {
      SPDLOG_WARN("[fp:{}] Action {} target type is ENNEMY, but Player {} tried to execute it with ally targets set",
                  _arena_id, action.action_name, user);
      return std::pair(false, target);
    }
    target = contender_target_id{action.contender_target.at(0)};
  } else if (t == data::targeting::ENNEMIES) {
    if (!action.ally_target.empty() || action.contender_target.empty()) {
      SPDLOG_WARN("[fp:{}] : Action {} target type is ENNEMIES, but Player {} tried to execute it with ally targets set",
                  _arena_id, action.action_name, user);
      return std::pair(false, target);
    }
    target = contenders_targets_ids{std::move(action.contender_target)};
  } else if (t == data::targeting::ALLY) {
    if (action.ally_target.empty() || !action.contender_target.empty()) {
      SPDLOG_WARN("[fp:{}] : Action {} target type is ALLY, but Player {} tried to execute it with contender targets set",
                  _arena_id, action.action_name, user);
      return std::pair(false, target);
    }
    target = ally_target_id{action.ally_target.at(0)};
  } else if (t == data::targeting::ALLIES) {
    if (action.ally_target.empty() || !action.contender_target.empty()) {
      SPDLOG_WARN("[fp:{}] : Action {} target type is ALLIES, but Player {} tried to execute it with contender targets set",
                  _arena_id, action.action_name, user);
      return std::pair(false, target);
    }
    target = allies_targets_ids{std::move(action.ally_target)};
  }
  //	else if (targetType == data::Targeting::ALLY_AND_ENNEMY) {}
  //	else if (targetType == data::Targeting::ALLY_OR_ENNEMY) {}
  else {
    SPDLOG_WARN("NOT IMPLEMENTED YET");
    return std::pair(false, target);
  }
  if (!fil::all_contains(action.ally_target, _party_teams.get_party_team_of_player(user).team_members(),
                         [](const team_member_sptr& tm) { return tm->id(); })) {
    SPDLOG_WARN("[fp:{}] : Player {} tried to target a non-existing Ally", _arena_id, user);
    return std::pair(false, target);
  }
  if (!fil::all_contains(action.contender_target, _contenders.get_contenders(),
                         [](const fighting_contender_sptr& c) { return c->id(); })) {
    SPDLOG_WARN("[fp:{}] : Player {} tried to target a non-existing Contender, Targets are : ", _arena_id, user);
    return std::pair(false, target);
  }
  return std::pair(true, target);
}
bool fighting_pit::add_contender(const std::shared_ptr<fighting_contender>& fc) {
  if (_contenders.add_contender(fc)) {
    _layout_map.add_active_contender();
    return true;
  }
  return false;
}

zmq::message_t
fighting_pit::make_winner_notification() const {
  flatbuffer_generator fb;
  auto [data, size] = fb.generate_end_battle(true, *_rewards);
  return zmq::message_t(data, size);
}

zmq::message_t
fighting_pit::make_looser_notification() const {
  flatbuffer_generator fb;
  auto [data, size] = fb.generate_end_battle(false, {});
  return zmq::message_t(data, size);
}

void fighting_pit::add_rewards(std::string action, uint quantity) noexcept {
  _rewards->keys.emplace_back(std::move(action));
  _rewards->quantity.emplace_back(quantity);
}

}// namespace fys::arena
