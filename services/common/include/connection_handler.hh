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

#ifndef FYS_ARENA_CONNECTIONHANDLER_HH
#define FYS_ARENA_CONNECTIONHANDLER_HH

#include <spdlog/spdlog.h>
#include <zmq_addon.hpp>

namespace fys::common {

class ArenaServerContext;

class connection_handler {

public:
  explicit connection_handler(
      zmq::socket_type type_socket = zmq::socket_type::dealer,
      int thread_number = 1) noexcept
      : _number_message(type_socket == zmq::socket_type::dealer ? 2 : 3),
        _zmq_context(thread_number),
        _dealer_connection_to_dispatcher(_zmq_context, type_socket) {}

  /**
   * @brief Connect to dispatcher in order to receive requests
   */
  void setup_connection_manager(const std::string& dispatcher_connection_str) noexcept;
  /**
   * @brief Reply to the dispatcher using the dealer (direct connection) socket
   */
  void send_msg_to_dispatcher(zmq::multipart_t&& msg) noexcept;

  /**
   * Read on the dealer socket (connection with the dispatcher/WS) and reply to dispatcher via this socket connection
   * (which will forward the message the the proper WorldServer)
   * @tparam Lambda type following the signature => void (string, zmq::message_t)
   * @param handler Handler handler to call when receiving a message
   */
  template<typename Handler>
  void poll_process_msg_from_dispatcher(Handler&& handler) noexcept {
    //  Initialize poll set
    zmq::pollitem_t items[] = {
        {_dealer_connection_to_dispatcher, 0, ZMQ_POLLIN, 0}};
    zmq::poll(&items[0], 1, 100);
    if (static_cast<bool>(items[0].revents & ZMQ_POLLIN)) {
      zmq::multipart_t msg;
      if (!msg.recv(_dealer_connection_to_dispatcher, ZMQ_NOBLOCK) || msg.size() != _number_message) {
        SPDLOG_ERROR("Error while reading on the listener socket");
      } else {
        // first frame is idt of the router of the dispatcher
        auto identity_for_dispatcher = msg.pop();
        // second frame is content
        std::forward<Handler>(handler)(std::move(identity_for_dispatcher), msg.pop());
      }
    }
  }

private:
  const std::uint32_t _number_message;
  zmq::context_t _zmq_context;

  // write to reply (forwarded to the world server)
  zmq::socket_t _dealer_connection_to_dispatcher;
};

}// namespace fys::common

#endif//FYS_ARENA_CONNECTIONHANDLER_HH
