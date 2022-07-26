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

#ifndef FYS_ARENASERVERCONTEXT_HH
#define FYS_ARENASERVERCONTEXT_HH

#include <array>

#include <nlohmann/json.hpp>

#include <service_context_base.hh>

namespace fys::arena {

struct encounter_context {
  using rng_range = std::pair<std::uint32_t, std::uint32_t>;
  using chance_array = std::array<std::uint32_t, 3>;

  struct encounter_desc {
    [[nodiscard]] bool
    operator!=(const encounter_desc& other) const {
      return std::make_tuple(key, maxEncountering, chance, levelRange) != std::make_tuple(other.key, other.maxEncountering, other.chance, other.levelRange);
    }

    //! key of the encounter to find it back via the cache
    std::string key;
    //! Max number of this type of monster you can encounter at once
    std::uint32_t maxEncountering;
    //! percentage of chance to encounter this monster out of the 3 different difficulties
    chance_array chance;
    //! Range of level the monster is when encountered
    rng_range levelRange;
  };

  struct reward_encounter_desc {
    //! range of items than can be dropped for an encounter out of the 3 different difficulties
    std::array<rng_range, 3> rangeDrop;
    //! map of items key over the chance of drop out of the 3 different difficulties
    std::map<std::string, chance_array> item_on_chance_range;
  };

  /**
   * Verify if the given zone is registered in the context
   * @param wsId zone to check
   * @return true if the zone is registered, false otherwise
   */
  [[nodiscard]] bool
  zoneRegistered(const std::string& wsId) const noexcept { return contenders_per_zone.find(wsId) != contenders_per_zone.cend(); }

  //! range of number of monster findable per zone
  std::map<std::string, std::array<rng_range, 3>> range_encounter_per_zone;
  //! contender findable per zone
  std::map<std::string, std::vector<encounter_desc>> contenders_per_zone;
  //! reward description per contender
  std::map<std::string, reward_encounter_desc> reward_desc_per_contender;
};

class arena_server_context : public fys::common::service_context_base {

  //! Minimum of number of battle in parallels that can be processed by the Arena Server
  static constexpr unsigned MINIMUM_BATTLE_THRESHOLD = 3;

public:
  arena_server_context(int ac, const char* const* av);

  [[nodiscard]] std::string
  to_string() const noexcept;

  [[nodiscard]] std::string
  player_binding_string() const noexcept;

  [[nodiscard]] const std::string&
  server_code() const noexcept { return _code; }

  [[nodiscard]] const encounter_context&
  encounter_ctx() const noexcept { return _encounter_ctx; }

  [[nodiscard]] const std::string&
  path_local_storage_cache() const noexcept { return _path_local_storage_cache; }

  [[nodiscard]] const std::string&
  path_source_cache() const noexcept { return _path_source_cache; }

  [[nodiscard]] const std::string&
  db_host() const noexcept { return _db_host; }

  [[nodiscard]] const std::string&
  get_history_saving_folder() const noexcept { return _path_history_saving_folder; };

  [[nodiscard]] std::uint32_t
  db_port() const noexcept { return _db_port; }

  [[nodiscard]] std::uint32_t
  battle_threshold() const noexcept { return _battle_threshold; }

private:
  [[nodiscard]] bool
  validate_encounter_context() const;

  [[nodiscard]] bool
  validate_reward_context() const;

  [[nodiscard]] encounter_context::reward_encounter_desc
  reward_desc_from_json(const nlohmann::json& rwd_desc) const;

  void parse_arena_config_file(const nlohmann::json& config_content);
  void parse_zone_config_file(const nlohmann::json& config_content);

private:
  //! Code of the current ArenaServerService
  std::string _code;

  //! Path representing the local temporary storage created and maintained by Cml
  std::string _path_local_storage_cache;
  //! Path of the reference data for CmlCopy
  std::string _path_source_cache;

  //! Path to the saving folder in which the history will be generated
  std::string _path_history_saving_folder;

  //! Database hostname
  std::string _db_host;
  //! Database port number
  std::uint32_t _db_port = 3306;

  //! Database hostname
  std::uint32_t _player_connection_port;

  //! Maximum number of concurrent battle handled by the arena service
  std::uint32_t _battle_threshold = MINIMUM_BATTLE_THRESHOLD;

  //! Context part specific to encounter handled by this arena server
  encounter_context _encounter_ctx;
};

}// namespace fys::arena

#endif// !FYS_ARENASERVERCONTEXT_HH
