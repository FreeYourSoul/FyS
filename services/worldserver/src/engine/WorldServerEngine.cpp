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

#include <WSAction_generated.h>
#include <Notifications_generated.h>
#include <ConnectionHandler.hh>

#include <WorldServerContext.hh>

#include <engine/WorldServerEngine.hh>
#include <FlatbufferGenerator.hh>

namespace fys::ws {

WorldServerEngine::WorldServerEngine(const fys::ws::WorldServerContext& ctx)
		:
		common::DirectConnectionManager(1, ctx.getPlayerConnectionString()),
		_map(ctx),
		_scriptEngine(ctx) { }

void
WorldServerEngine::authenticatePlayer(AuthPlayer auth, CharacterInfo info, std::string identifier)
{
	uint index = _data.addNewPlayerData(std::move(info), std::move(identifier), auth.userName);
	_authPlayerOnDataIndex.insert(std::pair(std::move(auth), index));
}

void
WorldServerEngine::setPlayerMoveDirection(uint index, double direction)
{
	_data.setPlayerMoveAction(index, direction);
}

void
WorldServerEngine::stopPlayerMove(uint index)
{
	_data.stopPlayerMove(index);
}

void
WorldServerEngine::executePendingMoves()
{
	_data.executeOnPlayers(
			[this](uint index, PlayerStatus status, CharacterInfo& pi, const std::string&, const std::string& userName) {
				if (status == PlayerStatus::MOVING) {
					movePlayerAction(userName, index, pi);
				}
			});

	_scriptEngine.executeScriptedActions(
			[this](uint index, CharacterInfo& pi, const NPCAction& action) {
				if (std::chrono::system_clock::now().time_since_epoch().count() < action.idleTime) {
					return index;
				}
				pi.angle = retrieveDirectionAngle(pi, action);
				moveNPCAction(index, pi);
				if (std::round(pi.pos.x) == std::round(action.destination.x) && std::round(pi.pos.y) ==
						std::round(action.destination.y)) {
					return index + 1;
				}
				return index;
			});
}

double
WorldServerEngine::retrieveDirectionAngle(CharacterInfo& info, const NPCAction& action) const
{
	return std::atan((info.pos.y - action.destination.y) / (info.pos.x - action.destination.x));
}

void
WorldServerEngine::spawnNPC(const std::chrono::system_clock::time_point& currentTime)
{
	_scriptEngine.spawnNewEncounters(currentTime);
}

void
WorldServerEngine::movePlayerAction(const std::string& userName, uint indexCharacter, CharacterInfo& pi)
{
	moveCharacterAction(userName, indexCharacter, pi, true);
}

void
WorldServerEngine::moveNPCAction(uint indexCharacter, CharacterInfo& pi)
{
	moveCharacterAction(std::string("NPC_").append(std::to_string(indexCharacter)), indexCharacter, pi, true);
}

void
WorldServerEngine::moveCharacterAction(const std::string& userName, uint indexCharacter, CharacterInfo& pi, bool isNpc)
{
	double velocity = pi.velocity;

	// calculate future position if the move occurs
	Pos futurePos = Pos{
			pi.pos.x * (velocity * std::cos(pi.angle)),
			pi.pos.y * (velocity * std::sin(pi.angle))
	};

	if (_map.canMoveTo(futurePos, 0)) {
		pi.pos = futurePos;

		// NPC Character can't trigger map triggers
		if (!isNpc) _map.executePotentialTrigger(indexCharacter, pi);

		if (const auto clientsToNotify = _data.getPlayerIdtsAroundPos(pi.pos); !clientsToNotify.empty()) {
			notifyClientsOfCharacterMove(pi, userName, clientsToNotify);
		}
	}
}

void
WorldServerEngine::notifyClientsOfCharacterMove(
		const CharacterInfo& pi,
		const std::string& userName,
		const std::vector<std::string_view>& idtsToNotify)
{
	const unsigned indexIdentity = 0;
	zmq::multipart_t toSend;
	FlatbufferGenerator fg;

	auto[binary, size] = fg.generateMoveNotification(userName, pi);
	toSend.add({});
	toSend.addmem(binary, size);
	for (const auto& id : idtsToNotify) {
		toSend.at(indexIdentity).rebuild(id.data(), id.size());
		toSend.send(_routerPlayerConnection);
	}
}

uint
WorldServerEngine::retrieveDataIndex(const AuthPlayer& player)
{
	auto it = _authPlayerOnDataIndex.find(player);
	if (it == _authPlayerOnDataIndex.end()) {
		return NOT_AUTHENTICATED;
	}
	return _authPlayerOnDataIndex.at(player);
}

}
