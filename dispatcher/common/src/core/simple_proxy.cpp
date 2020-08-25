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

#include <context/startup_dispatcher_ctx.hh>

#include "core/simple_proxy.hh"

namespace fys {

simple_proxy::simple_proxy(const startup_dispatcher_ctx &ctx) noexcept:
	_ctx(1),
	_frontend(_ctx, zmq::socket_type::router),
	_backend(_ctx, zmq::socket_type::dealer) {
  _frontend.bind(ctx.listener_binding_str());
  _backend.bind(ctx.dispatcher_binding_str());
}

void simple_proxy::start_proxy() {
  zmq::proxy(zmq::socket_ref(_frontend), zmq::socket_ref(_backend));
}

}