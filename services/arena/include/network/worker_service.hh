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
#include <utility>
#include <zmq_addon.hpp>
#include <fightingPit/fighting_pit.hh>

// forward declarations
namespace fys::cache {
class Cml;
}
namespace fys::arena {
class party_team;
class arena_server_context;
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
class worker_service {

	inline static constexpr unsigned SERVER_FULL_CAPACITY = std::numeric_limits<unsigned>::max();

	struct player_identifier {
		std::string user_name;
		std::string identifier;
	};

public:
	explicit worker_service();

	void start_fighting_pits_loop();

	/**
	 * @memberof
	 * @brief Bind the ArenaService for the player to connect directly to the Arena
	 */
	void setup_connection_manager(const fys::arena::arena_server_context& ctx) noexcept;

	/**
	 * @brief Add a fighting pit to the worker service, adding this instance to the on-going/accessible fighting pit
	 * of the arena server. Create an id for the newly created arena and set it to the fighting pit.
	 *
	 * @param fp fighting_pit to add in the current worker_service
	 * @return newly added fighting_pit id, or 0 (FightingPit::CREATION_ERROR) if an error occurred
	 */
	[[nodiscard]] unsigned
	add_fighting_pit(std::unique_ptr<fighting_pit> fp);

	/**
	 * Player join a given fighting pit
	 * @param fighting_pit_id id of the fighting pit to join
	 * @param pt party team retrieved to add
	 * @param cml cache manager used to register chaiscript scripts
	 */
	void player_join_fighting_pit(unsigned fighting_pit_id, std::unique_ptr<party_team> pt, cache::Cml& cml);

	/**
	 * Broadcast a message containing all the information about the incoming players data
	 * - UserName
	 * - TeamMembers id, name, stats (life, magic points)
	 * - // todo cosmetic information (equipment)
	 *
	 * @note Checks about the validity of the userName and the fightingPitId given as parameter have to be done beforehand
	 *
	 * @param fp_id id of the fighting pit, HAS TO BE ACCURATE
	 * @param user_name user whom is going to
	 */
	void send_msg_new_arriving_team(unsigned fp_id, const std::string& user_name) noexcept;

	/**
	 * Read on the router socket (connection with the players) and reply to them thanks to the zmq::router protocol
	 *
	 * @note not using common::direct_connection_manager as there is an intermediate frame used for authentication
	 *
	 * @tparam Lambda type following the signature => void (string, zmq::message_t)
	 * @param HandlerAuth   Handler handler to call when receiving a message to authenticate an awaited player
	 * @param HandlerInGame Handler handler to call when receiving a message to do a player action on a fightingPit
	 */
	template<typename HandlerAuth, typename HandlerInGame>
	void poll_and_process_msg_from_player(HandlerAuth&& handler_auth, HandlerInGame&& handler_in_game) noexcept
	{
		//  Initialize poll set
		zmq::pollitem_t items[] = {
				{_worker_router, 0, ZMQ_POLLIN, 0}
		};
		zmq::poll(&items[0], 1, 100);
		if (static_cast<bool>(items[0].revents & ZMQ_POLLIN)) {
			zmq::multipart_t msg;
			if (!msg.recv(_worker_router, ZMQ_NOBLOCK)) {
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
					std::forward<HandlerAuth>(handler_auth)(std::move(identity), std::move(content));
				}
				else {
					std::forward<HandlerInGame>(handler_in_game)(std::move(identity), intermediate, std::move(content));
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
	 * @param fp_id arena to check if the player is authenticated on
	 * @return A reference to the fighting pit if the player is authenticated on the given fightingPit,
	 * 		   return std::nullopt otherwise.
	 */
	[[nodiscard]] std::optional<std::reference_wrapper<fighting_pit>>
	get_authenticated_player_fighting_pit(const std::string& name, const std::string& token, unsigned fp_id);

	[[nodiscard]] const std::unique_ptr<fighting_pit>&
	get_fighting_pit_instance(unsigned arena_id) const { return _arena_instances.at(arena_id); }

	[[nodiscard]] std::pair<bool, bool>
	fighting_pit_exist_and_joinable(unsigned fighting_pit_id) const noexcept;

	[[nodiscard]] unsigned
	get_number_battle_running() const noexcept { return _arena_instances.size(); }

	/**
	 * Update or add (if player not register yet) the player identifier mapping with fightingPit
	 * @param fp_id key of the mapping, multiple players are linked with a fightingpit
	 * @param user_name unique name of the player
	 * @param idt_player network identity of the player
	 */
	void upsert_player_identifier(unsigned fp_id, std::string user_name, std::string idt_player);

	void direct_send_msg_to_player(zmq::message_t&& identity, zmq::message_t&& msg);

private:
	bool
	broadcast_msg(unsigned fp_id, zmq::message_t&& msg, const std::string& except = "");

	bool
	send_msg_to_player(unsigned fp_id, const std::string& user_name, zmq::message_t&& msg);

	[[nodiscard]] auto
	broadcast_msg_handler(unsigned fp_id)
	{
		return [this, fp_id](zmq::message_t&& to_send) { this->broadcast_msg(fp_id, std::move(to_send)); };
	}

	[[nodiscard]] const std::string&
	retrieve_player_identifier(unsigned fp_id, const std::string& user_name);

	void cleanup_finished_battles();

private:
	zmq::context_t _ctx;
	zmq::socket_t _worker_router;
	unsigned _current_arena_id;

	// map of fighting pit id with identifiers
	std::unordered_map<unsigned, std::vector<player_identifier>> _arena_id_on_identifier;
	std::unordered_map<unsigned, std::unique_ptr<fighting_pit>> _arena_instances;

};

}

#endif // !FYS_WORKERSERVICE_HH
