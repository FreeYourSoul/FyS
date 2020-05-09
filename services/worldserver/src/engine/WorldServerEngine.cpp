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
#include <WorldServerContext.hh>
#include <WSAction_generated.h>
#include <Notifications_generated.h>
#include <ConnectionHandler.hh>
#include "engine/WorldServerEngine.hh"

namespace fys::ws {

WorldServerEngine::WorldServerEngine(const fys::ws::WorldServerContext& ctx)
		:_map(ctx)
{
}

void
WorldServerEngine::executePendingActions(ws::ConnectionHandler& conn)
{
	_data.executeOnPlayers([this, &conn](uint indexPlayer, PlayerStatus statusPlayer, PlayerInfo& pi, const std::string& identityPlayer) {
		if (statusPlayer == PlayerStatus::MOVING) {
			movePlayerAction(identityPlayer, indexPlayer, pi, conn);
		}
	});
}

void
WorldServerEngine::processPlayerInputMessage(std::string&& idt, std::string&& token,
		const fb::WSAction* actionMsg, ConnectionHandler& handler)
{
	if (const uint index = _data.getIndexAndUpdatePlayerConnection(token, std::move(idt));
			index < std::numeric_limits<uint>::max()) {
		if (actionMsg->action_type() == fb::Action::Action_Move) {
			_data.setPlayerMoveAction(index, actionMsg->action_as_Move()->direction());
		}
		else if (actionMsg->action_type() == fb::Action::Action_Interruption) {
//                if (auto moveAction = actionMsg->action_as_Move(); moveAction->enterArena())
//                    _data.setPlayerArena(index, moveAction->arenaId());
//                else
			_data.stopPlayerMove(index);
		}
	}
	else {
		SPDLOG_ERROR("Player of token {} has not been registered before sending messages", token);
	}
}

void
WorldServerEngine::movePlayerAction(const std::string& idt, uint indexPlayer, PlayerInfo& pi, ws::ConnectionHandler& conn)
{
	double velocity = 1;
	Pos futurePos = Pos{
			pi.pos.x * (velocity * std::cos(pi.angle)),
			pi.pos.y * (velocity * std::sin(pi.angle))
	};

	if (_map.canMoveTo(futurePos, 0)) {
		pi.pos = futurePos;
		_map.executePotentialTrigger(indexPlayer, pi, conn);
		if (const auto clientsToNotify = _data.getPlayerIdtsArroundPos(pi); !clientsToNotify.empty()) {
			notifyClientsOfMove(clientsToNotify, conn);
		}
	}
}

void
WorldServerEngine::notifyClientsOfMove(const std::vector<std::string_view>& ids, ws::ConnectionHandler& conn) const
{
	for (const auto& id : ids) {
		// TODO: notify
	}
}

}
