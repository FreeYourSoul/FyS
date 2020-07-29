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

#include <string>

#include <fmt/format.h>

#include "chat_engine.hh"

namespace fys::chat {


// Key Value Database

std::string key_for_player(const std::string &user_name) {
return fmt::format(FMT_STRING("fys#chat#player#{}"), user_name);
}

std::string key_for_room(const std::string &room_name) {
  return fmt::format(FMT_STRING("fys#chat#room#{}"), room_name);
}

std::string key_for_listing_players_in_room(const std::string &room_name) {
  return fmt::format(FMT_STRING("{}#players#"), key_for_room(room_name));
}

std::string key_for_player_in_room(const std::string &room_name, const std::string &user_name) {
  return fmt::format(FMT_STRING("{}{}"), key_for_listing_players_in_room(room_name), user_name);
}

}