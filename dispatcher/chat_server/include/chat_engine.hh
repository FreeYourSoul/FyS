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

#ifndef FYS_ONLINE_SERVICES_CHAT_SRC_CHAT_ENGINE_HH
#define FYS_ONLINE_SERVICES_CHAT_SRC_CHAT_ENGINE_HH

#include <string>

namespace fys::chat {

/**
 * The chat room engine register all chat room that are created,
 * It can be a chat default chat room for servers (update purpose, global information)
 * It can be a custom chat room created by players in order to communicate together in the game.
 *
 * In order to create the default chat room,
 * Some can be created at startup of the application thanks to a configuration file used to setup
 * the context of the server.
 * Others are going to be created when the chat_engine will retrieve information about the current
 * layout of world_servers in the
 *
 */
class chat_engine {

};

inline std::string key_for_player(const std::string &user_name);
inline std::string key_for_room(const std::string &room_name);
inline std::string key_for_listing_players_in_room(const std::string &room_name);
inline std::string key_for_player_in_room(const std::string &room_name, const std::string &user_name);

}

#endif //FYS_ONLINE_SERVICES_CHAT_SRC_CHAT_ENGINE_HH
