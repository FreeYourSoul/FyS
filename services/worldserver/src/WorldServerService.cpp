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

#include <spdlog/spdlog.h>
#include <zmq_addon.hpp>
#include <flatbuffers/flatbuffers.h>
#include <WSAction_generated.h>
#include <AuthFrame_generated.h>
#include <Notifications_generated.h>
#include "WorldServerService.hh"

namespace {
template<typename T>
[[nodiscard]] bool
verifyBuffer(const void* fbBuffer, uint size)
{
	auto v = flatbuffers::Verifier(static_cast<const uint8_t*>(fbBuffer), size);
	return v.VerifyBuffer<T>();
}
}

namespace fys::ws {

WorldServerService::WorldServerService(const WorldServerContext& ctx)
		:_worldServer(ctx)
{
	_connectionHandler.setupConnectionManager(ctx);

}

void
WorldServerService::runServerLoop() noexcept
{
	SPDLOG_INFO("WorldServer loop started");

	while (true) {
		_connectionHandler.pollAndProcessSubMessage(
				// Inter-Server communication
				[this](zmq::message_t&& identity, zmq::message_t&& content) {

				},

				// Auth-Server Incoming player
				[this](zmq::message_t&& identity, zmq::message_t&& authFrameMsg, zmq::message_t&& content) {

					if (!verifyBuffer<fb::AuthFrame>(authFrameMsg.data(), authFrameMsg.size())) {
						SPDLOG_ERROR("Ill formatted Authentication frame");
						return;
					}
					auto* authFrame = fb::GetAuthFrame(authFrameMsg.data());

					SPDLOG_DEBUG("message received with idt='{}', token='{}'", idt, token);
				}
		);

		_worldServer.executePendingActions(_connectionHandler);
	}
}

}