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

#include "engine/player_data.hh"
#include <spdlog/spdlog.h>

namespace fys::ws {

player_data::player_data(std::uint32_t max_connection) noexcept {
  _positions.reserve(max_connection);
  _status.reserve(max_connection);
  _identities.reserve(max_connection);
  _userNames.reserve(max_connection);
}

std::vector<std::string_view>
player_data::get_player_idts_around_player(std::uint32_t index_player,
                                           std::optional<std::reference_wrapper<character_info>> position,
                                           double distance) const noexcept {
  if (position.has_value() && index_player < _positions.size()) {
    return get_player_idts_around_pos(position->get().position, distance, index_player);
  }
  return get_player_idts_around_pos(_positions.at(index_player).position, distance, index_player);
}

std::vector<std::string_view>
player_data::get_player_idts_around_pos(const pos& position,
                                        double distance,
                                        std::uint32_t ignore_index) const noexcept {
  std::vector<std::string_view> player_identities;
  player_identities.reserve(LIMIT_NOTIFICATIONS_MOVE);
  for (std::size_t i = 0; i < _positions.size(); ++i) {
    if (player_identities.size() >= LIMIT_NOTIFICATIONS_MOVE) {
      break;
    }
    if (i == ignore_index) {
      continue;
    }
    pos min_position = _positions.at(i).position - distance;
    pos max_position = _positions.at(i).position + distance;

    if ((position.x > min_position.x && position.x < max_position.x) && (position.y > min_position.y && position.x < max_position.y)) {
      player_identities.emplace_back(_identities.at(i));
    }
  }
  player_identities.shrink_to_fit();
  return player_identities;
}

uint player_data::add_new_player_data(character_info info, std::string identity, std::string user_name) {
  if (!((_identities.size() == _status.size()) == (_positions.size() == _userNames.size()))) {
    SPDLOG_CRITICAL("MISMATCH, all vectors require to be equal idt:'{}' stat:'{}' pos:'{}', names:'{}'",
                    _identities.size(), _status.size(), _positions.size(), _userNames.size());
    return 0;
  }
  std::uint32_t index = _identities.size();
  _positions.emplace_back(std::move(info));
  _status.emplace_back(player_status::STANDING);
  _identities.emplace_back(std::move(identity));
  _userNames.emplace_back(std::move(user_name));
  return index;
}

}// namespace fys::ws
