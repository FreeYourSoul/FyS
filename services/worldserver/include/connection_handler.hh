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

#ifndef FYS_CONNECTIONHANDLER_HH
#define FYS_CONNECTIONHANDLER_HH

#include "world_server_context.hh"
#include <spdlog/spdlog.h>
#include <zmq_addon.hpp>

namespace fys::ws {

class connection_handler {

  //! Subscriber channel used by server to communicate via the dispatcher
  inline static const std::string SERVER_SUB_CHANNEL_KEY = "Server_Inter_Com";

public:
  explicit connection_handler(std::uint32_t thread_number = 1) noexcept;

  void setup_connection_manager(const fys::ws::world_server_context& ctx) noexcept;
  void send_msg_to_dispatcher(zmq::multipart_t&& msg) noexcept;

  template<typename HandlerIncoming, typename HandlerInterServer>
  void poll_and_process_sub_msg(HandlerIncoming&& handler_incoming, HandlerInterServer&& handler_server) noexcept {
    //  Initialize poll set
    zmq::pollitem_t items[] = {
        {_sub_socket_on_dispatcher, 0, ZMQ_POLLIN, 0}};
    zmq::poll(&items[0], 1, 10);
    if (static_cast<bool>(items[0].revents & ZMQ_POLLIN)) {
      zmq::multipart_t msg;
      if (!msg.recv(_sub_socket_on_dispatcher, ZMQ_NOBLOCK) || (msg.size() != 3 && msg.size() != 4)) {
        SPDLOG_ERROR("Error while reading on the listener socket.");
        SPDLOG_ERROR("Received message may be ill formatted, contains '{}' part, message is : {}",
                     msg.size(), msg.str());
      } else {
        // first  frame is subscription channel
        const std::string subKey = msg.popstr();
        // second frame is identity
        auto identity = msg.pop();

        // third frame is the message content

        if (SERVER_SUB_CHANNEL_KEY == subKey) {
          std::forward<HandlerInterServer>(handler_server)(std::move(identity), msg.pop());
          return;
        }
        std::forward<HandlerIncoming>(handler_incoming)(std::move(identity), msg.pop());
      }
    }
  }

private:
  zmq::context_t _zmq_ctx;
  zmq::socket_t _sub_socket_on_dispatcher;
  zmq::socket_t _deal_socket_on_dispatcher;
};

}// namespace fys::ws

#endif//FYS_CONNECTIONHANDLER_HH
