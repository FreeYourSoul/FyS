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
#include <utility>
#include <zmq_addon.hpp>
#include <flatbuffers/flatbuffers.h>
#include <WSAction_generated.h>
#include <AuthFrame_generated.h>
#include <IncomingPlayerOnWs_generated.h>
#include <InterServerCom_generated.h>
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
		:_ctx(ctx), _worldServer(ctx)
{
	_connectionHandler.setupConnectionManager(ctx);
}

void
WorldServerService::runServerLoop() noexcept
{
	SPDLOG_INFO("WorldServer loop started");

	while (true) {
		_connectionHandler.pollAndProcessSubMessage(
				// Auth-Server Incoming player
				// Used by the Auth server in order to authenticate a player
				[this](zmq::message_t&& identity, zmq::message_t&& content) {
					if (verifyBuffer<fb::auth::IncomingPlayerOnWs>(content.data(), content.size())) {
						SPDLOG_ERROR("Wrongly formatted IncomingPlayerOnWs buffer");
						return;
					}
					auto* frame = fb::auth::GetIncomingPlayerOnWs(content.data());

					_awaitedIncomingPlayer.emplace_back(
							AwaitedPlayer{
									AuthPlayer{frame->userName()->str(), frame->token()->str()},
									Pos{frame->posX(), frame->posY()},
									frame->angle(),
									frame->velocity()
							}
					);
				},

				// Inter-Server communication
				// Used when world server are communicating between each others to handle player transition
				[this](zmq::message_t&& identity, zmq::message_t&& content) {
					if (verifyBuffer<fb::world::InterServerCom>(content.data(), content.size())) {
						SPDLOG_ERROR("Wrongly formatted InterServerCom buffer");
						return;
					}
				}
		);

		_worldServer.pollAndProcessPlayerMessage(

				// direct player communication
				// Used when a player is communicating an interaction with the world_server
				[this](zmq::message_t&& identity, zmq::message_t&& authFrameMsg, zmq::message_t&& content) {
					if (!verifyBuffer<fb::world::AuthFrame>(authFrameMsg.data(), authFrameMsg.size())) {
						SPDLOG_ERROR("Ill formatted Authentication frame");
						return;
					}
					if (!verifyBuffer<fb::world::WSAction>(content.data(), content.size())) {
						SPDLOG_ERROR("Ill formatted Authentication frame");
						return;
					}
					auto* authFrame = fb::world::GetAuthFrame(authFrameMsg.data());
					auto* frame = fb::world::GetWSAction(content.data());

					if (frame->action_type() == fb::world::Action_ValidateAuth) {
						registerAwaitedPlayer(authFrame->userName()->str(), authFrame->token()->str(), std::string());
						return;
					}

					uint index = _worldServer.retrieveDataIndex({authFrame->userName()->str(), authFrame->token()->str()});

					if (index == NOT_AUTHENTICATED) {
						SPDLOG_ERROR("Player '{}' isn't authenticated", authFrame->userName()->str());
						return;
					}

					switch (frame->action_type()) {
						case fb::world::Action_StopMove:
							_worldServer.stopPlayerMove(index);
							break;
						case fb::world::Action_Move:
							_worldServer.setPlayerMoveDirection(index, frame->action_as_Move()->direction());
							break;
						case fb::world::Action_PnjInteract:
						case fb::world::Action_JoinArena:
							SPDLOG_INFO("NOT_IMPLEMENTED YET");
							break;
						default:
							SPDLOG_ERROR("This action isn't handled by WorldServer Service '{}'", frame->action_type());
					}

					SPDLOG_DEBUG("message received with idt='{}', token='{}'", idt, token);
				}
		);

		_worldServer.executePendingMoves(_connectionHandler);
	}
}

void
WorldServerService::registerAwaitedPlayer(const std::string& user, const std::string& token, std::string identity)
{
	auto[isAwaited, awaitedIt] = isPlayerAwaited(user, token);
	if (!isAwaited) {
		SPDLOG_ERROR("Player '{}' isn't awaited with the given token", user, token);
		return;
	}

	_worldServer.authenticatePlayer(
			awaitedIt->auth,
			PlayerInfo{awaitedIt->initialPosition, awaitedIt->initialVelocity, awaitedIt->initialAngle},
			std::move(identity)
	);
}

std::pair<bool, WorldServerService::AwaitingPlayerWorldServerIt>
WorldServerService::isPlayerAwaited(const std::string& userName, const std::string& token) const
{
	auto it = std::find_if(_awaitedIncomingPlayer.begin(), _awaitedIncomingPlayer.end(),
			[toCheck = AuthPlayer{userName, token}](const auto& awaited) {
				return awaited.info == toCheck;
			});
	if (it == _awaitedIncomingPlayer.end()) {
		return std::pair(false, _awaitedIncomingPlayer.begin());
	}
	return std::pair(true, it);
}

}