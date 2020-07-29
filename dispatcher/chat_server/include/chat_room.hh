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

#ifndef FYS_ONLINE_SERVICES_CHAT_INCLUDE_CHAT_ROOM_HH
#define FYS_ONLINE_SERVICES_CHAT_INCLUDE_CHAT_ROOM_HH

namespace fys::chat {

enum class room_type {
  //! default room for zone
  ws_zone,

  //! default room for a WorldServer
  ws_server,

  //! temporary chat room used in a battle (a time to live is set by default on it)
  arena,

  //! player defined room
  custom
};

/**
 * A Chat room is defined by its uuid,
 * A uuid is something that can be manually created by a user in order to create a custom defined chatting room
 * to have private chat with other players, for default room, it is generated at startup of the application.
 */
class chat_room {

  struct internal;

public:
  chat_room(std::string room_uuid, room_type type);
  ~chat_room();

  void set_room_description(std::string description);

private:
	std::unique_ptr<internal> _intern;

};

}
#endif //FYS_ONLINE_SERVICES_CHAT_INCLUDE_CHAT_ROOM_HH
