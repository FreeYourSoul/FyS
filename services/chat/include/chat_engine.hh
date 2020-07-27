/**
 *
 * Copyright (C) 2020 INAIT.SA - - All Rights Reserved
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 *
 * Proprietary and confidential
 *
 **/

#ifndef FYS_ONLINE_SERVICES_CHAT_SRC_CHAT_ENGINE_HH
#define FYS_ONLINE_SERVICES_CHAT_SRC_CHAT_ENGINE_HH

namespace fys::chat {

/**
 * The chat room engine register all chat room that are created,
 * It can be a chat default chat room for servers (update purpose, global informations)
 * It can be a custom chat room created by players in order to communicate together in the game.
 *
 * In order to create the default chat room,
 * Some can be created at startup of the application thanks to a configuration file used to setup
 * the context of the server.
 * Others are going to be created when the chat_engine will retrieve informations about the current
 * layout of world_servers in the
 *
 */
class chat_engine {

};

}

#endif //FYS_ONLINE_SERVICES_CHAT_SRC_CHAT_ENGINE_HH
