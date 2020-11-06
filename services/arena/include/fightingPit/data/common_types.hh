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

#ifndef FYS_COMMONDATA_HH
#define FYS_COMMONDATA_HH

#include <fightingPit/hexagon_side.hh>
#include <functional>
#include <set>
#include <vector>

//! Types and functions linking C++ world with ChaiScript
namespace fys::arena::data {
static constexpr bool CONTENDER = true;
static constexpr bool PARTY_MEMBER = false;

[[nodiscard]] inline std::string
get_action_name_from_key(const std::string &key) {
  auto start_separator = key.find_last_of(':');
  auto end_separator = key.find_last_of('.');

  if (start_separator != std::string::npos && end_separator != std::string::npos) {
	// if a ':' and a '.' are found
	return key.substr(start_separator + 1, end_separator - start_separator - 1);
  } else if (start_separator == std::string::npos && end_separator != std::string::npos) {
	// if a ':' is not found but not a '.' is found
	return key.substr(0, end_separator);
  } else if (start_separator != std::string::npos && end_separator == std::string::npos) {
	// if a ':' is found but not a '.' is not found
	return key.substr(start_separator + 1);
  }
  return key;
}

[[nodiscard]] inline std::string
get_alteration_name_from_key(const std::string &key) {
  std::string prefix = "alteration_";
  return prefix.append(get_action_name_from_key(key));
}

struct priority_elem {// Improve with strong typing on ID/SPEED
  priority_elem() = default;
  priority_elem(std::uint32_t id, int speed, bool is_contender)
	  : id(id), speed(speed), is_contender(is_contender) {}

  std::uint32_t id;
  int speed;
  bool is_contender = false;

  [[nodiscard]] priority_elem &
  operator-(const priority_elem &other) {
	speed -= other.speed;
	return *this;
  }

  [[nodiscard]] bool
  operator<(const priority_elem &other) const {
	return speed < other.speed;
  }

  [[nodiscard]] bool
  operator==(const priority_elem &other) const {
	return id == other.id && is_contender == other.is_contender;
  }
};

struct life {
  std::uint32_t current = 0;
  std::uint32_t total = 0;

  [[nodiscard]] bool
  is_dead() const { return current == 0; }
};

struct magic_point {
  std::uint32_t current = 0;
  std::uint32_t total = 0;
};

struct status;

/**
 * Representation of a status alteration of a character
 */
class alteration {
public:
  alteration(std::string alteration_key, std::uint32_t lvl, std::uint32_t turn, std::function<int(data::status &, uint, uint)> a) noexcept
	  : _alteration_key(std::move(alteration_key)),
		_lvl(lvl),
		_turn(turn),
		_action(std::move(a)) {}

  /**
   * Process an alteration action, return if it has an impact on the turn of the player
   * @return false if the player turn is skipped, true otherwise
   */
  bool process_alteration(status &status) {
	if (_action && _turn > 0) {
	  --_turn;
	  return _action(status, _lvl, _turn);
	}
	return true;
  }

  const std::string &alteration_key() const { return _alteration_key; }
  std::uint32_t turn() const { return _turn; }

private:
  std::string _alteration_key;
  std::uint32_t _lvl;
  std::uint32_t _turn;
  std::function<int(status &, uint, uint)> _action;
};

struct status {
  life life_pt;
  magic_point magic_pt;
  std::uint32_t initial_speed;

  /**
   * When moving the appropriate flag is set in FightingPitLayout which use
   * this member to set the new position of the character
   */
  hexagon_side::orientation moving = hexagon_side::orientation::NONE;

  std::vector<alteration> alteration_before;
  std::vector<alteration> alteration_after;

  //! Alteration executed on the turn (usually stat modification impacting the damages/heal)
  std::vector<alteration> alterations;

  /**
   * Process all alteration before turn stored
   * @return true if the turn is executed, false if it is cancelled
   */
  bool process_alteration_before_turn() {
	bool processTurn = true;
	for (auto &alteration : alteration_before) {
	  processTurn &= alteration.process_alteration(*this);
	}
	return processTurn;
  }

  void process_alteration_after_turn() {
	for (auto &alteration : alteration_after) {
	  alteration.process_alteration(*this);
	}
  }

  void cleanup_finished_alteration() {
	alterations.erase(std::remove_if(alterations.begin(), alterations.end(),
									 [](const auto &alt) { return alt.turn() <= 0; }),
					  alterations.end());
	alteration_before.erase(std::remove_if(alteration_before.begin(), alteration_before.end(),
										   [](const auto &alt) { return alt.turn() <= 0; }),
							alteration_before.end());
	alteration_after.erase(std::remove_if(alteration_after.begin(), alteration_after.end(),
										  [](const auto &alt) { return alt.turn() <= 0; }),
						   alteration_after.end());
  }
};

/**
 * Merge the alterations, remove doubles, but keep the alteration that would last the longest.
 * @param to_modify reference on the alterations vector to modify
 * @param to_merge additional alternations
 * @param replace_if_exist replace the alteration if it is already existing if set at true,
 * do not add the alteration otherwise
 */
static void
merge_alterations(std::vector<alteration> &to_modify, std::vector<alteration> to_merge, bool replace_if_exist = false) {
  (void)merge_alterations;// suppress unused warning (as it is used, but by ChaiScript)

  std::move(to_merge.begin(), to_merge.end(), std::back_inserter(to_modify));
  std::sort(to_modify.begin(), to_modify.end(), [replace_if_exist](const alteration &lhs, const alteration &rhs) {
	return lhs.alteration_key() == rhs.alteration_key() && ((replace_if_exist && lhs.turn() > rhs.turn()) || (!replace_if_exist && lhs.turn() < rhs.turn()));
  });
  to_modify.erase(std::unique(to_modify.begin(), to_modify.end(), [](const alteration &lhs, const alteration &rhs) {
					return lhs.alteration_key() == rhs.alteration_key();
				  }),
				  to_modify.end());
}

enum move_direction {
  BACK,
  RIGHT,
  LEFT
};

enum targeting {
  SELF = 0,
  ENNEMY = 1,
  ALLY = 2,
  ENNEMIES = 3,
  ALLIES = 4,
  ALLY_AND_ENNEMY = 5,
  ALLY_OR_ENNEMY = 6,
  SIDE = 7
};

}// namespace fys::arena::data

#endif// !FYS_COMMONDATA_HH
