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

#ifndef FYS_FIGHTINGPITANNOUNCER_HH
#define FYS_FIGHTINGPITANNOUNCER_HH

#include <arena_server_context.hh>
#include <fightingPit/contender/pit_contenders.hh>
#include <fightingPit/fighting_pit.hh>
#include <fightingPit/rewards.hh>
#include <fightingPit/team/ally_party_teams.hh>
#include <memory>
#include <optional>
#include <vector>

// forward declarations
namespace fys::cache {
class cml;
}

namespace fys::arena {

/**
 * @brief Builder class to create the FightingPit.
 *   - It generate the position of the Pit participants (contenders and team)
 *   - Define if the fight is an ambush or not.
 *   - Download the required scripts for the contenders (if not in the cache).
 *   - Download the required scripts for the team (if not in the cache)
 *   - Set the level of the fighting pit
 *
 */
class fighting_pit_announcer {

public:
  enum encounter_type {
    RANDOM,
    SCRIPTED
  };

public:
  explicit fighting_pit_announcer(cache::cml& cml);
  ~fighting_pit_announcer();

  /**
   * Build a fighting pit thanks to the information given
   * @param ctx encounter context supplied at the startup of the application (contains informations about the type
   * of encounter a player can find depending on zones.
   *
   * @param ws_id world server id on which the generation of the fighting pit takes place
   * @return a newly generated fighting pit
   */
  [[nodiscard]] std::unique_ptr<fighting_pit>
  build_fighting_pit(const encounter_context& ctx, const std::string& ws_id);

  void set_encounter_type(encounter_type type) {
    if (type == RANDOM)
      _id_encounter = 0;
    _encounter_type = type;
  }

  void set_encounter_id(std::uint32_t id_encounter) {
    if (_encounter_type == RANDOM && !id_encounter)
      _encounter_type = SCRIPTED;
    _id_encounter = id_encounter;
  }

  /**
   * It is assumed that the player defined by the username/token is already correctly authenticated before
   * calling this method (call this method only if the username/token is valid).
   * @param creator_user_name name of the authenticated player generating a fighting pit
   */
  void set_creator_user_name(std::string creator_user_name) { _creator_user_name = std::move(creator_user_name); }

  /**
   * It is assumed that the player defined by the username/token is already correctly authenticated before
   * calling this method (call this method only if the username/token is valid).
   * @param creator_user_token token of the authenticated player generating a fighting pit
   */
  void set_creator_user_token(std::string creator_user_token) { _creator_user_token = std::move(creator_user_token); }
  void enforce_ambush(bool ambush_enforced) noexcept { _is_ambush_enforced = ambush_enforced; }
  void set_difficulty(fighting_pit::level level) noexcept { _difficulty = level; }
  void set_creator_team_party(std::unique_ptr<party_team> pt) noexcept;
  void set_join_disabled(bool is_join_disabled) noexcept { _is_join_disabled = is_join_disabled; }

  // ============================================================================================================================
  // for testing validation purpose
  [[nodiscard]] static const ally_party_teams&
  get_party_teams(const std::unique_ptr<fighting_pit>& fp);

  [[nodiscard]] static const pit_contenders&
  get_pit_contenders(const std::unique_ptr<fighting_pit>& fp) { return fp->_contenders; }

  [[nodiscard]] static const std::string&
  creator_user_name(const std::unique_ptr<fighting_pit>& fp) { return fp->_creator_user_name; }

  [[nodiscard]] static side_battle&
  get_side_battle_for_side(const std::unique_ptr<fighting_pit>& fp, hexagon_side::orientation side);

  [[nodiscard]] static unsigned
  get_arena_id(const std::unique_ptr<fighting_pit>& fp) { return fp->_arena_id; }

  [[nodiscard]] static const rewards&
  get_reward(const std::unique_ptr<fighting_pit>& fp) { return *fp->_rewards; }

  [[nodiscard]] static bool
  is_on_going(const std::unique_ptr<fighting_pit>& fp) { return fp->_progress == fighting_pit::progress::ON_GOING; }
  [[nodiscard]] static bool
  is_ally_win(const std::unique_ptr<fighting_pit>& fp) { return fp->_progress == fighting_pit::progress::ALLY_WIN; }
  [[nodiscard]] static bool
  is_contender_win(const std::unique_ptr<fighting_pit>& fp) { return fp->_progress == fighting_pit::progress::CONTENDER_WIN; }
  [[nodiscard]] static const std::vector<side_battle>&
  get_side_vector(const std::unique_ptr<fighting_pit>& fp) { return fp->_side_battles; }

  // just for testing purpose
  void add_action_to_one_member(std::uint32_t index, const std::string& action_name, std::uint32_t level);
  void set_interval(const std::unique_ptr<fighting_pit>& fp, std::chrono::milliseconds interval) { fp->_time_interlude = interval; }

private:
  [[nodiscard]] inline const std::string&
  get_script_content_string(std::string name, const encounter_context::encounter_desc& desc);

  [[nodiscard]] bool
  is_scripted_encounter() const { return _encounter_type != encounter_type::RANDOM; }

  [[nodiscard]] bool
  is_random_encounter() const { return _encounter_type == encounter_type::RANDOM; }

  [[nodiscard]] inline bool
  generate_contenders(fighting_pit& fp, const encounter_context& ctx, const std::string& wsId);

  inline void generate_reward_for_contender(fighting_pit& fp, const encounter_context& ctx,
                                            const std::vector<std::shared_ptr<fighting_contender>>& contenders);

private:
  cache::cml& _cache;

  /**
   *   range           desc
   *     0    : random encounter generation
   * [1001-*] : globally defined encounters
   * [1-1000] : zone defined encounters
   */
  std::uint32_t _id_encounter = 0;
  fighting_pit::level _difficulty = fighting_pit::level::MEDIUM;
  encounter_type _encounter_type = encounter_type::RANDOM;

  std::optional<bool> _is_ambush_enforced;

  std::vector<std::string> _loaded_script;

  std::string _creator_user_name;
  std::string _creator_user_token;
  bool _is_join_disabled = false;

  std::unique_ptr<party_team> _creator_party_team;
};

}// namespace fys::arena

#endif// !FYS_FIGHTINGPITANNOUNCER_HH
