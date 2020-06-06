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
#include <engine/ScriptEngine.hh>
#include <FlatbufferGenerator.hh>

namespace fys::ws {

WorldServerEngine::WorldServerEngine(const std::string& playerConnectionStr,
		CollisionMap&& map,
		std::unique_ptr<ScriptEngine>&& scriptEngine,
		std::chrono::system_clock::duration timeInterval)
		:
		common::DirectConnectionManager(1, playerConnectionStr),
		_map(std::move(map)),
		_scriptEngine(std::move(scriptEngine)),
		_nextTick(std::chrono::system_clock::now() + timeInterval) { }

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
WorldServerEngine::executePendingMoves(const std::chrono::system_clock::time_point& now)
{
	// Don't process pending moves if it isn't ticking
	if (now < _nextTick) {
		return;
	}
	// Setup next tick time point
	_nextTick = _nextTick + TIMING_MOVE_INTERVAL;

	_data.executeOnPlayers(
			[this](uint playerIndex, PlayerStatus status, CharacterInfo& pi, const std::string&, const std::string& userName) {
				if (status == PlayerStatus::MOVING) {
					movePlayerAction(userName, playerIndex, pi);
				}
			});

	_scriptEngine->executeEncounterScriptedActions();
	_scriptEngine->executeNeutralScriptedActions();
}

void
WorldServerEngine::spawnNPC(const std::chrono::system_clock::time_point& currentTime)
{
	_scriptEngine->spawnNewEncounters(currentTime);
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
