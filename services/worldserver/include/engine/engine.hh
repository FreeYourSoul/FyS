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
#include <chrono>

#include <zmq_addon.hpp>

#include <WSAction_generated.h>

#include <engine/player_data.hh>
#include <engine/collision_map.hh>
#include <engine/world_populator.hh>
#include <direct_connection_manager.hh>

// forward declaration
namespace fys::ws {
class script_engine;
}
namespace fys::fb {
struct WSAction;
}
// end forward declarations

namespace fys::ws {

constexpr static std::uint32_t NOT_AUTHENTICATED = std::numeric_limits<uint>::max();

struct auth_player {
	std::string user_name;
	std::string token;

	bool operator==(const auth_player& other) const
	{
		return user_name == other.user_name && token == other.token;
	}
	bool operator<(const auth_player& other) const
	{
		return user_name < other.user_name && token < other.token;
	}
};

class engine : public common::direct_connection_manager {

	friend class world_populator;

public:
	explicit engine(const std::string& player_connect_str, collision_map&& map,
			std::shared_ptr<script_engine> script_engine, std::chrono::system_clock::duration time_interval);
	~engine();
	engine(engine&&) noexcept;
	engine(const engine&) = delete;

	void execute_pending_moves(const std::chrono::system_clock::time_point& player_index);
	void set_player_move_direction(std::uint32_t index, double direction);
	void stop_player_move(std::uint32_t index);
	void authenticate_player(auth_player auth, character_info info, std::string identifier);
	void spawnNPC(const std::chrono::system_clock::time_point& currentTime);

	[[nodiscard]] uint
	retrieve_data_index(const auth_player& player);

private:
	inline void move_player_action(const std::string& user_name, std::uint32_t index_character, character_info& pi);
	inline void move_npc_action(std::uint32_t index_character, character_info& pi);
	inline void move_character_action(const std::string& user_name, std::uint32_t index_character, character_info& pi, bool is_npc);

	inline void notifyClientsOfCharacterMove(const character_info& pi, const std::string& user_name,
			const std::vector<std::string_view>& idts_to_identify);

private:
	collision_map _map;
	player_data _data;

	//! Engine managing scripts of NPC and map triggers
	std::unique_ptr<script_engine> _script_engine;

	//! Next movement tick should take place at this moment
	std::chrono::system_clock::time_point _next_tick;

	//! Authenticated user token to index in PlayersData
	std::map<auth_player, uint> _auth_player_on_data_index;

};

}

#endif //FYS_WORLDSERVERENGINE_HH
