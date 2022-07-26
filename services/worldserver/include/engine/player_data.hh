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

#ifndef FYS_PLAYERSDATA_HH
#define FYS_PLAYERSDATA_HH

#include <array>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fys::util {
class key;
}

namespace fys::ws {

//! Default distance to check character around a point
constexpr static double DEFAULT_DISTANCE = 30;
//! Maximum number of character notified when a character move
constexpr static unsigned LIMIT_NOTIFICATIONS_MOVE = 50;

template<typename T>
struct vec2 {
  using unity_type = T;

  unity_type x = 0;
  unity_type y = 0;

  vec2 operator+(const vec2& other) const {
    return {x + other.x, y + other.y};
  }

  vec2 operator+(unity_type value) const {
    return {x + value, y + value};
  }

  vec2 operator-(const vec2& other) const {
    return {x - other.x, y - other.y};
  }

  vec2 operator-(unity_type value) const {
    return {x - value, y - value};
  }
};

template<typename T>
struct rectangle final {
  using unity_type = T;

  rectangle()
      : left(0), top(0), width(0), height(0) {}
  rectangle(unity_type l, unity_type t, unity_type w, unity_type h)
      : left(l), top(t), width(w), height(h) {}
  rectangle(vec2<unity_type> position, vec2<unity_type> size)
      : left(position.x), top(position.y), width(size.x), height(size.y) {}
  unity_type left, top, width, height;
};

using hitbox_d = rectangle<double>;
using hitbox_u = rectangle<uint>;
using vec2_u = vec2<uint>;
using vec2_d = vec2<double>;

//! Position in 2D space (X, Y)
using pos = vec2<double>;

struct character_info {
  pos position;
  double velocity = 1.0;
  double angle = 0.0;// in radiant
};

enum class player_status {
  FIGHTING,
  MOVING,
  STANDING
};

class player_data {

  static constexpr std::uint32_t MAXIMUM_PLAYER_CONNECTED_TO_WORLD_SERVER = 1000;

public:
  explicit player_data(std::uint32_t max_connection = MAXIMUM_PLAYER_CONNECTED_TO_WORLD_SERVER) noexcept;

  template<typename Action>
  void execution_on_player(Action&& action_to_execute) {
    for (std::uint32_t i = 0; i < _status.size(); ++i) {
      std::forward<Action>(action_to_execute)(i, _status.at(i), _positions.at(i), _identities.at(i), _userNames.at(i));
    }
  }

  void set_player_move_action(std::uint32_t index, double direction) {
    _positions.at(index).angle = direction;
    _status.at(index) = player_status::MOVING;
  }

  void stop_player_move(std::uint32_t index) {
    _status.at(index) = player_status::STANDING;
  }

  void set_player_status_in_arena(std::uint32_t index, [[maybe_unused]] const std::string& arena_id) {
    _status.at(index) = player_status::FIGHTING;
  }

  [[nodiscard]] uint
  add_new_player_data(character_info info, std::string identity, std::string user_name);

  /**
   * @brief Get the Players Identities Around the given player except himself
   *
   * @param index_player player
   * @param position
   * @param distance
   * @return std::vector<std::string_view>
   */
  [[nodiscard]] inline std::vector<std::string_view>
  get_player_idts_around_player(std::uint32_t index_player,
                                std::optional<std::reference_wrapper<character_info>> position,
                                double distance = DEFAULT_DISTANCE) const noexcept;

  /**
   * @brief Get the Players Identities Around the given position
   *
   * @param position to check around
   * @param distance radius around the given point to search
   * @param ignore_index index that need to be ignored in the return
   * @return std::vector<std::string_view> vector of view on the identities of the players around the given point
   * @note identities are zmq code to reply to a specific client via a router socket
   */
  [[nodiscard]] std::vector<std::string_view>
  get_player_idts_around_pos(const pos& position,
                             double distance = DEFAULT_DISTANCE,
                             std::uint32_t ignore_index = LIMIT_NOTIFICATIONS_MOVE) const noexcept;

private:
  std::vector<character_info> _positions;
  std::vector<player_status> _status;
  std::vector<std::string> _identities;
  std::vector<std::string> _userNames;
};

}// namespace fys::ws

#endif//FYS_PLAYERSDATA_HH
