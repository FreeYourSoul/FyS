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

#ifndef FYS_WORKERSERVICE_HH
#define FYS_WORKERSERVICE_HH

#include <spdlog/spdlog.h>
#include <zmq_addon.hpp>
#include <fightingPit/FightingPit.hh>

// forward declarations
namespace fys::cache {
class Cml;
}
namespace fys::arena {
class PartyTeam;
class ArenaServerContext;
}

namespace fys::arena {

/**
 * Manage the fighting pits :
 *   - Generate a fighting pit
 *   - Assign an ID to it
 *   - Assign a dedicated thread to it
 *
 * Fighting pit are created by the WorkerService using the FightingPitAnnouncer as builder in order to
 * ensure that the fightingpit is ready to host a battle, and then start the fighting pit in a dedicated
 * thread.
 *
 * Each fighting pit will contains a DEALER socket that will be used to communicate with the WorkerService.
 * A custom zmq identifier will be provided by the WorkerService in order to be able to target a specific
 * FightingPit for specific players (the map _idOnArenaId being used to register which player is in which
 * FightingPit).
 *
 * @see fys::arena::FightingPitAnnouncer
 * @see fys::arena::FightingPit
 */
class WorkerService {

	struct PlayerIdentifier {
		std::string userName;
		std::string identifier;
	};

public:
	explicit WorkerService();

	void startFightingPitsLoop();

	/**
	 * @memberof
	 * @brief Bind the ArenaService for the player to connect directly to the Arena
	 */
	void setupConnectionManager(const fys::arena::ArenaServerContext& ctx) noexcept;

	/**
	 * @brief Add a fighting pit to the worker service, adding this instance to the on-going/accessible fighting pit
	 * of the arena server. Create an id for the newly created arena and set it to the fighting pit.
	 *
	 * @param fp FightingPit to add in the current WorkerService
	 * @return newly added fighting pit id, or 0 (FightingPit::CREATION_ERROR) if an error occurred
	 */
	[[nodiscard]] unsigned
	addFightingPit(std::unique_ptr<FightingPit> fp);

	/**
	 * Player join a given fighting pit
	 * @param fightingPitId id of the fighting pit to join
	 * @param pt party team retrieved to add
	 * @param cml cache manager used to register chaiscript scripts
	 */
	void playerJoinFightingPit(unsigned fightingPitId, std::unique_ptr<PartyTeam> pt, cache::Cml& cml);

	/**
	 * Broadcast a message containing all the information about the incoming players data
	 * - UserName
	 * - TeamMembers id, name, stats (life, magic points)
	 * - // todo cosmetic information (equipment)
	 *
	 * @note Checks about the validity of the userName and the fightingPitId given as parameter have to be done beforehand
	 *
	 * @param fpId id of the fighting pit, HAS TO BE ACCURATE
	 * @param userName user whom is going to
	 */
	void sendMsgNewArrivingTeam(unsigned fpId, const std::string& userName) noexcept;

	/**
	 * Read on the router socket (connection with the players) and reply to them thanks to the zmq::router protocol
	 * @tparam Lambda type following the signature => void (string, zmq::message_t)
	 * @param HandlerAuth   Handler handler to call when receiving a message to authenticate an awaited player
	 * @param HandlerInGame Handler handler to call when receiving a message to do a player action on a fightingPit
	 */
	template<typename HandlerAuth, typename HandlerInGame>
	void pollAndProcessMessageFromPlayer(HandlerAuth&& handlerAuth, HandlerInGame&& handlerInGame) noexcept
	{
		//  Initialize poll set
		zmq::pollitem_t items[] = {
				{_workerRouter, 0, ZMQ_POLLIN, 0}
		};
		zmq::poll(&items[0], 1, 100);
		if (static_cast<bool>(items[0].revents & ZMQ_POLLIN)) {
			zmq::multipart_t msg;
			if (!msg.recv(_workerRouter, ZMQ_NOBLOCK)) {
				SPDLOG_ERROR("Error while reading on the arena worker listener socket");
			}
			else if (auto s = msg.size(); s != 3) {
				SPDLOG_WARN("An incoming message with {} instead of 3 has been read", s);
			}
			else {
				auto identity = msg.pop();
				auto intermediate = msg.pop();
				auto content = msg.pop();
				if ("auth" == intermediate.to_string_view()) {
					std::forward<HandlerAuth>(handlerAuth)(std::move(identity), std::move(content));
				}
				else {
					std::forward<HandlerInGame>(handlerInGame)(std::move(identity), intermediate, std::move(content));
				}
			}
		}
	}

	/**
	 * Check if the player defined by an unique name/token is authenticated on the given fightingArenaId
	 * And return the given fightingpit if it's the case
	 *
	 * @param name player name
	 * @param token authentication token
	 * @param fightingPitId arena to check if the player is authenticated on
	 * @return A reference to the fighting pit if the player is authenticated on the given fightingPit,
	 * 		   return std::nullopt otherwise.
	 */
	[[nodiscard]] std::optional<std::reference_wrapper<FightingPit>>
	getAuthenticatedPlayerFightingPit(const std::string& name, const std::string& token, unsigned fightingPitId);

	[[nodiscard]] const std::unique_ptr<FightingPit>&
	getFightingPitInstance(unsigned arenaId) const { return _arenaInstances.at(arenaId); }

	[[nodiscard]] std::pair<bool, bool>
	fightingPitExistAndJoinable(unsigned fightingPitId) const noexcept;

	[[nodiscard]] unsigned
	getNumberBattleRunning() const noexcept { return _arenaInstances.size(); }

	/**
	 * Update or add (if player not register yet) the player identifier mapping with fightingPit
	 * @param fightingPitId key of the mapping, multiple players are linked with a fightingpit
	 * @param userName unique name of the player
	 * @param idtPlayer network identity of the player
	 */
	void upsertPlayerIdentifier(unsigned fightingPitId, std::string userName, std::string idtPlayer);

private:
	bool
	broadcastMsg(unsigned fpId, zmq::multipart_t& msg);

	bool
	sendMessageToPlayer(unsigned fpId, const std::string& userName, zmq::multipart_t& msg);

	[[nodiscard]] const std::string&
	retrievePlayerIdentifier(unsigned fpId, const std::string& userName);

	void cleanUpFinishedBattles();

private:
	zmq::context_t _ctx;
	zmq::socket_t _workerRouter;
	unsigned _currentArenaId;

	// map of fighting pit id with identifiers
	std::unordered_map<unsigned, std::vector<PlayerIdentifier>> _arenaIdOnIdentifier;
	std::unordered_map<unsigned, std::unique_ptr<FightingPit>> _arenaInstances;

};

}

#endif // !FYS_WORKERSERVICE_HH
