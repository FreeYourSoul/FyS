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

#include "ConnectionHandler.hh"

namespace fys::ws {

ConnectionHandler::ConnectionHandler(int threadNumber) noexcept
		:
		_zmqContext(threadNumber),
		_subSocketOnDispatcher(_zmqContext, zmq::socket_type::sub),
		_dealSocketOnDispatcher(_zmqContext, zmq::socket_type::dealer)
{
}

void
ConnectionHandler::setupConnectionManager(const fys::ws::WorldServerContext& ctx) noexcept
{
	_subSocketOnDispatcher.set(zmq::sockopt::subscribe, ctx.getServerCode());
	_subSocketOnDispatcher.set(zmq::sockopt::subscribe, SERVER_SUB_CHANNEL_KEY);
	_subSocketOnDispatcher.connect(ctx.getDispatcherSubConnectionString());
	_dealSocketOnDispatcher.connect(ctx.getDispatcherConnectionString());
}

void
ConnectionHandler::sendMessageToDispatcher(zmq::multipart_t&& msg) noexcept
{
	if (_dealSocketOnDispatcher.connected()) {
		msg.send(_dealSocketOnDispatcher);
	}
}

}
