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

#ifndef FYS_ONLINE_DIRECT_CONNECTION_MANAGER_HH
#define FYS_ONLINE_DIRECT_CONNECTION_MANAGER_HH

#include <spdlog/spdlog.h>
#include <zmq_addon.hpp>

namespace fys::common {

/**
 * @brief A connection manager that will handle direct connection from the final user (player).
 *
 */
class direct_connection_manager {

public:
  direct_connection_manager(unsigned threadNumber, const std::string& bindingString)
      : _ctx(threadNumber), _router_player_connection(_ctx, zmq::socket_type::router) {
    _router_player_connection.bind(bindingString);
  }

  /**
   * An envelope of message received is split into three distinct parts :
   *  - identity frame : consumed by this connection manager in order to be used by ZMQ to properly redirect any reply
   *  - authentication frame : consumed by this connection manager in order to verify if the client is properly authenticated
   *  - payload : message forwarded to the generic handler provided.
   *
   * @param handler_player handler that will receive the payload of the messaged polled
   */
  template<typename HandlerPlayer>
  void pollAndProcessPlayerMessage(HandlerPlayer&& handler_player) {
    //  Initialize poll set
    zmq::pollitem_t items[] = {
        {_router_player_connection, 0, ZMQ_POLLIN, 0}};
    zmq::poll(&items[0], 1, 10);
    if (static_cast<bool>(items[0].revents & ZMQ_POLLIN)) {
      zmq::multipart_t msg;
      if (!msg.recv(_router_player_connection, ZMQ_NOBLOCK) || (msg.size() != 3)) {
        SPDLOG_ERROR("Error while reading on the listener socket.");
        SPDLOG_ERROR("Received message may be ill formatted, contains '{}' part, message is : {}",
                     msg.size(), msg.str());
      } else {
        // first frame is the identity
        auto identity = msg.pop();
        // second frame is auth frame of the player
        auto auth_frame = msg.pop();
        std::forward<HandlerPlayer>(handler_player)(std::move(identity), std::move(auth_frame), msg.pop());
      }
    }
  }

protected:
  zmq::context_t _ctx;

  //! Socket direct connection with the player
  zmq::socket_t _router_player_connection;
};

}// namespace fys::common

#endif//FYS_ONLINE_DIRECT_CONNECTION_MANAGER_HH
