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

#include "../include/connection_handler.hh"

namespace fys::ws {

connection_handler::connection_handler(std::uint32_t thread_number) noexcept
	: _zmq_ctx(thread_number),
	  _sub_socket_on_dispatcher(_zmq_ctx, zmq::socket_type::sub),
	  _deal_socket_on_dispatcher(_zmq_ctx, zmq::socket_type::dealer) {
}

void connection_handler::setup_connection_manager(const fys::ws::world_server_context &ctx) noexcept {
  _sub_socket_on_dispatcher.set(zmq::sockopt::subscribe, ctx.server_code());
  _sub_socket_on_dispatcher.set(zmq::sockopt::subscribe, SERVER_SUB_CHANNEL_KEY);
  _sub_socket_on_dispatcher.connect(ctx.dispatcher_sub_connection_str());
  _deal_socket_on_dispatcher.connect(ctx.dispatcher_connection_str());
}

void connection_handler::send_msg_to_dispatcher(zmq::multipart_t &&msg) noexcept {
  if (_deal_socket_on_dispatcher.connected()) {
	msg.send(_deal_socket_on_dispatcher);
  }
}

}// namespace fys::ws
