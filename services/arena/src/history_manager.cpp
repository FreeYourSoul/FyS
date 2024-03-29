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

#include <fstream>

#include <nlohmann/json.hpp>
#include <fmt/format.h>

#include <fightingPit/fighting_pit.hh>

#include <logger.hh>
#include "history_manager.hh"

namespace {

[[nodiscard]] std::string
target_str(std::optional<fys::arena::target_type>) {
  return "";
}

// helper type for the visitor
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

[[nodiscard]] nlohmann::json
generate_json_target(fys::arena::history_action history_action) {
  const std::optional<fys::arena::target_type>& target = history_action.targets;

  if (target.has_value()) {
    auto vis = overloaded{
        [](fys::arena::hexagon_side::orientation target) { return std::vector({fys::arena::to_string(target)}); },
        [](const fys::arena::contender_target_id& target) { return std::vector({std::to_string(target.v)}); },
        [](const fys::arena::ally_target_id& target) { return std::vector({std::to_string(target.v)}); },
        [](const auto& targets) {
          std::vector<std::string> result;
          result.reserve(targets.v.size());
          for (const unsigned v : targets.v) {
            result.emplace_back(std::to_string(v));
          }
          return result;
        }};
    return {
        {"id", history_action.id_character},
        {"isContender", false},
        {"targets", std::visit(vis, target.value())}};
  }

  return {
      {"id", history_action.id_character},
      {"isContender", false}};
}

}// namespace

namespace fys::arena {

void history_manager::add_historic(unsigned fp_id, history_action&& ha) {
  if (get_instance()._is_manager_on) {
    auto it = get_instance()._history.find(fp_id);
    if (it != get_instance()._history.end()) {

      log_info(fmt::format("[fp:{}] : Member {}.{} execute action '{}' targeting {}", fp_id,
                           ha.id_character, ha.name, ha.action_key, target_str(ha.targets)));

      it->second.player_actions.emplace_back(std::move(ha));
    }
  }
}

void history_manager::activate_historic_manager(bool on) {
  get_instance()._is_manager_on = on;
}

void history_manager::set_to_be_saved(std::uint32_t pit_id, bool toBeSaved) {
  auto& instance = get_instance();
  if (instance._is_manager_on) {
    auto it = instance._history.find(pit_id);
    if (it != instance._history.end()) {
      it->second.has_to_be_saved = toBeSaved;
    }
  }
}

void history_manager::create_historic(const fighting_pit& fp, unsigned seed) {
  auto& instance = get_instance();
  if (instance._is_manager_on) {
    history_fight to_add(fp.id(), seed);
    for (const auto& pt : fp.ally_party().get_party_teams()) {
      // todo : initial setup in history of the party teams
    }
    for (const auto& c : fp.contenders().get_contenders()) {
      // todo : initial setup in history of the contenders
    }
    instance._history[fp.id()] = std::move(to_add);
  }
}

void history_manager::save(std::uint32_t fp_id) {
  auto& instance = get_instance();
  if (!instance._is_manager_on) {
    return;
  }
  auto history_it = instance._history.find(fp_id);
  if (history_it == instance._history.end()) {
    return;
  }
  auto& history_elem = history_it->second;

  if (history_elem.has_to_be_saved && history_elem.seed > 0) {
    log_info(fmt::format("[fp:] HistoryAction : Save history file for fight id {}", history_elem.id));
    auto date = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    nlohmann::json actions;

    for (const auto& pa : history_elem.player_actions) {
      actions["actions"].push_back(nlohmann::json{
          {"id", pa.id_character},
          {"isContender", false},
          {"actionKey", pa.action_key},
          generate_json_target(pa)});
    }

    nlohmann::json json = {
        {"date", std::ctime(&date)},
        actions};

    std::ofstream file(history_elem.generate_history_filename(instance._path_to_save_folder));
    file << json;
  }
  get_instance()._history.erase(history_it);
}

void history_manager::set_save_path(std::string save_path) {
  auto path = std::filesystem::path(save_path) / "arena_fight_history";
  std::error_code ec;

  if (std::filesystem::exists(path)) {
    std::filesystem::rename(
        path, std::filesystem::path(save_path)           //
                  / fmt::format("arena_fight_history_{}",//
                                std::to_string(std::chrono::system_clock::now().time_since_epoch().count())),
        ec);
  }
  std::filesystem::create_directories(path, ec);

  get_instance()._path_to_save_folder = std::move(save_path);
}

std::string history_manager::history_fight::generate_history_filename(const std::filesystem::path& base_path) const {
  return (base_path / fmt::format("id_{}-seeded_{}.fhs", id, seed)).string();
}

}// namespace fys::arena
