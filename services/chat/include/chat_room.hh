/**
 *
 * Copyright (C) 2020 INAIT.SA - - All Rights Reserved
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 *
 * Proprietary and confidential
 *
 **/

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
