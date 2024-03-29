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

#ifndef FYS_ONLINE_SIMPLEPROXY_HH
#define FYS_ONLINE_SIMPLEPROXY_HH

#include <zmq.hpp>

namespace fys {

class startup_dispatcher_ctx;

class simple_proxy {

public:
  explicit simple_proxy(const startup_dispatcher_ctx& ctx) noexcept;

  void start_proxy();

private:
  zmq::context_t _ctx;
  zmq::socket_t _frontend;
  zmq::socket_t _backend;
};

}// namespace fys

#endif//FYS_ONLINE_SIMPLEPROXY_HH
