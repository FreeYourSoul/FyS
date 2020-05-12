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


#ifndef FYS_WORLDSERVERENGINE_HH
#define FYS_WORLDSERVERENGINE_HH

#include <utility>
#include <map>

#include <zmq_addon.hpp>

#include <WSAction_generated.h>

#include <engine/PlayersData.hh>
#include <engine/CollisionMap.hh>
#include <engine/ScriptEngine.hh>


// forward declaration
namespace fys::fb {
struct WSAction;
}
namespace fys::ws {
class WorldServerContext;
class CollisionMap;
class WorldPopulator;
}
// end forward declarations

namespace fys::ws {

constexpr static uint NOT_AUTHENTICATED = std::numeric_limits<uint>::max();

struct AuthPlayer {
	std::string userName;
	std::string token;

	bool operator==(const AuthPlayer& other) const
	{
		return userName == other.userName && token == other.token;
	}
	bool operator<(const AuthPlayer& other) const
	{
		return userName < other.userName && token < other.token;
	}
};

class WorldServerEngine {

	friend class WorldPopulator;

public:
	explicit WorldServerEngine(const WorldServerContext& ctx);

	void executePendingMoves();
	void setPlayerMoveDirection(uint index, double direction);
	void stopPlayerMove(uint index);
	void authenticatePlayer(AuthPlayer auth, PlayerInfo info, std::string identifier);

	[[nodiscard]] uint
	retrieveDataIndex(const AuthPlayer& player);

	template<typename HandlerPlayer>
	void pollAndProcessPlayerMessage(HandlerPlayer&& handlerPlayer)
	{
		//  Initialize poll set
		zmq::pollitem_t items[] = {
				{_routerPlayerConnection, 0, ZMQ_POLLIN, 0}
		};
		zmq::poll(&items[0], 1, 10);
		if (static_cast<bool>(items[0].revents & ZMQ_POLLIN)) {
			zmq::multipart_t msg;
			if (!msg.recv(_routerPlayerConnection, ZMQ_NOBLOCK) || (msg.size() != 3)) {
				SPDLOG_ERROR("Error while reading on the listener socket.");
				SPDLOG_ERROR("Received message may be ill formatted, contains '{}' part, message is : {}",
						msg.size(), msg.str());
			}
			else {
				// first frame is the identity
				auto identity = msg.pop();
				// second frame is auth frame of the player
				auto authFrame = msg.pop();
				std::forward<HandlerPlayer>(handlerPlayer)(std::move(identity), std::move(authFrame), msg.pop());
			}
		}
	}

private:

	inline void notifyClientsOfMove(const PlayerInfo& pi, const std::string& userName,
			const std::vector<std::string_view>& idtsToNotify);

	inline void movePlayerAction(const std::string& idt, uint indexPlayer, PlayerInfo& pi);

private:
	CollisionMap _map;
	PlayersData _data;

	zmq::context_t _zmqCtx;
	zmq::socket_t _routerPlayerConnection;

	//! Engine managing scripts of NPC and map triggers
	ScriptEngine _scriptEngine;

	//! Authenticated user token to index in PlayersData
	std::map<AuthPlayer, uint> _authPlayerOnDataIndex;

};

}

#endif //FYS_WORLDSERVERENGINE_HH
