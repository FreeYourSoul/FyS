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

#ifndef FYS_ARENASERVERSERVICE_HH
#define FYS_ARENASERVERSERVICE_HH

#include <unordered_map>
#include <optional>
#include <network/DBConnector.hh>
#include <network/WorkerService.hh>
#include <ConnectionHandler.hh>
#include <CmlCopy.hh>

// forward declarations
namespace fys::fb {
struct ArenaServerValidateAuth;
struct ArenaFightAction;
}
// !end forward declarations

namespace fys::arena {

class ArenaServerContext;

/**
 * @brief contains the information related to the arena that has to be generated
 */
struct AwaitingArena {
	std::string serverCode;
	bool isJoinDisabled;
	bool isAmbush;
	unsigned encounterId;
	FightingPit::Level levelFightingPit;
};

/**
 * @brief A Player is awaited whom could generate a new FightingPit if _fightingPitId is not set
 * AwaitingPlayerArena represent a player that is the trigger of a new fighting pit or a join action of an existing
 * one, it contains the information related to the arena that has to be generated (none required when joining).
 */
struct AwaitingPlayerArena {
	std::string namePlayer;
	std::string token;

	// if set, a new arena is generated at the client authentication, otherwise the player just join the fightingpit
	unsigned fightingPitId = 0;

	// in case of generation, data of the fighting pit to generate
	std::optional<AwaitingArena> gen;

	[[nodiscard]] bool
	hasToBeGenerated() const
	{
		return fightingPitId == 0 && static_cast<bool>(gen);
	}

};

/**
 * @brief Class managing an Arena Server.
 *<br/>
 * An arena server is a server handling multiple arena fighting pit instance at once.
 * In order to properly works, it need to work with an ArenaDispatcher, which is a proxy for the WorldServer to
 * communicate with Arena.
 *
 * fys::arena::ArenaServerService::createNewFightingPit In case of a new encounter for a player, the following workflow apply:
 * - Player is moving / doing an action on the WorldMap (managed by a WorldServer) that is triggering a new
 *     encounter. [publisher socket send notification]
 *     <br/><br/>
 *
 * - WorldServer send a message to a ArenaDispatcher containing configuration data to generate a fighting pit and
 *     authentication information, those are the following:<br/>
 *          * a generated token (used as authentication key)<br/>
 *          * the difficulty of the fight (configuration of the player)<br/>
 *          * the id of encounter; if equal to 0, it is a random encounter. scripted id otherwise.<br/>
 * - The Dispatcher forward the message to an ArenaServer that is going to register the incoming player that will
 *   initiate the creation of the fighting pit.
 *   <br/><br/>
 *
 * - The Arena server reply to the dispatcher (that will reply to the WorldServer), the message will return to the
 *   player, message containing the ip (connection string) of the ArenaServer that will create the fighting pit.
 *   <br/><br/>
 *
 * - The Player can then directly connect to the ArenaServer using the token to validate the authentication, The
 *   Arena service validate the connection, and use the data (difficulty ect..) in order to generate the
 *   fighting pit (with an id unique). When the fight is setup, the initial handshake with the client contains the
 *   id of the fighting pit in order to enable other player to join.
 *   <br/>
 *
 * @joinFightingPit Another player than the creator can join an existing arena by specifying its fighting pit id to
 * the WorldServer, which will check if the position of the joining player is close enough of the creator of the
 * fighting pit. The ArenaServer is then just going to check the existence of the id and will add the player into
 * the fight.
 *
 */
class ArenaServerService {
	using AwaitingPlayerArenaIt = const std::unordered_map<std::string, AwaitingPlayerArena>::const_iterator;

	//! action name to set readiness
	static inline const std::string READY_ACTION = "READY";
	//! magic id to set readiness
	static inline const uint READY_ACTION_ID = 1337;

public:
	explicit ArenaServerService(const ArenaServerContext& ctx);

	/**
	 * @brief Run infinite loop that poll on the connections of the dispatcher, then of the players.
	 * @note This method contains the code of the deserialization of flatbuffer message (ioc with lambda) and then
	 *       check if the incoming message is coming from an authorized user.
	 */
	void runServerLoop() noexcept;

private:
	/**
	 * Verify if the server is saturated (too many battle at the same time running on this ArenaService)
	 * @return true if the server is saturated, false otherwise
	 */
	[[nodiscard]] inline bool
	isSaturated() const noexcept;

	[[nodiscard]] PlayerAction
	createPlayerAction(std::string&& action, const fb::ArenaFightAction* frame) const;

	[[nodiscard]] unsigned
	createNewFightingPit(const AwaitingPlayerArena& awaited) noexcept;

	[[nodiscard]] std::pair<bool, AwaitingPlayerArenaIt>
	isPlayerAwaited(const std::string& name, const std::string& token, unsigned idFightingPit) const noexcept;

	inline void sendSaturatedErrorMsg(zmq::message_t&& identity);
	inline void forwardReplyToDispatcher(zmq::message_t&& idtWs, const fys::arena::AwaitingPlayerArena& awaitArena) noexcept;


private:
	std::reference_wrapper<const ArenaServerContext> _ctx;
	cache::CmlCopy _cache;

	// Manage connections
	std::unique_ptr<network::DBConnector> _dbConnector;
	ConnectionHandler _connectionHandler;

	// Manage fighting pits
	WorkerService _workerService;

	// map of token on awaiting arena
	std::unordered_map<std::string, AwaitingPlayerArena> _awaitingArena;
};

}

#endif // !FYS_ARENASERVERSERVICE_HH
