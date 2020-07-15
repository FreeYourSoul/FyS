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
#include <connection_handler.hh>

#include <world_server_context.hh>

#include <engine/engine.hh>
#include <engine/script_engine.hh>
#include <flatbuffer_generator.hh>

namespace fys::ws {

engine::engine(const std::string& player_connect_str,
		collision_map&& map,
		std::shared_ptr<script_engine> engine,
		std::chrono::system_clock::duration time_interval)
		:
		common::direct_connection_manager(1, player_connect_str),
		_map(std::move(map)),
		_script_engine(std::make_unique<script_engine>(std::move(*engine))), // beurk...
		_next_tick(std::chrono::system_clock::now() + time_interval) { }

engine::~engine() = default;
engine::engine(engine&&) noexcept = default;

void
engine::authenticate_player(auth_player auth, character_info info, std::string identifier)
{
	std::uint32_t index = _data.add_new_player_data(std::move(info), std::move(identifier), auth.user_name);
	_auth_player_on_data_index.insert(std::pair(std::move(auth), index));
}

void
engine::set_player_move_direction(std::uint32_t index, double direction)
{
	_data.set_player_move_action(index, direction);
}

void
engine::stop_player_move(std::uint32_t index)
{
	_data.stop_player_move(index);
}

void
engine::execute_pending_moves(const std::chrono::system_clock::time_point& player_index)
{
	// Don't process pending moves if it isn't ticking
	if (player_index < _next_tick) {
		return;
	}
	// Setup next tick time point
	_next_tick = _next_tick + TIMING_MOVE_INTERVAL;

	_data.execution_on_player(
			[this](std::uint32_t playerIndex, player_status status, character_info& pi, const std::string&, const std::string& userName) {
				if (status == player_status::MOVING) {
					move_player_action(userName, playerIndex, pi);
				}
			});

	_script_engine->execute_encounter_scripted_actions();
	_script_engine->execute_neutral_scripted_actions();
}

void
engine::spawnNPC(const std::chrono::system_clock::time_point& currentTime)
{
	_script_engine->spawn_new_encounters(currentTime);
}

void
engine::move_player_action(const std::string& user_name, std::uint32_t index_character, character_info& pi)
{
	move_character_action(user_name, index_character, pi, true);
}

void
engine::move_npc_action(std::uint32_t index_character, character_info& pi)
{
	move_character_action(std::string("NPC_").append(std::to_string(index_character)), index_character, pi, true);
}

void
engine::move_character_action(const std::string& user_name, std::uint32_t index_character, character_info& pi, bool is_npc)
{
	double velocity = pi.velocity;

	// calculate future position if the move occurs
	pos futurePos = pos{
			pi.position.x * (velocity * std::cos(pi.angle)),
			pi.position.y * (velocity * std::sin(pi.angle))
	};

	if (_map.can_move_to(futurePos, 0)) {
		pi.position = futurePos;

		// NPC Character can't trigger map triggers
		if (!is_npc) _map.execute_potential_trigger(index_character, pi);

		if (const auto clientsToNotify = _data.get_player_idts_around_pos(pi.position); !clientsToNotify.empty()) {
			notifyClientsOfCharacterMove(pi, user_name, clientsToNotify);
		}
	}
}

void
engine::notifyClientsOfCharacterMove(
		const character_info& pi,
		const std::string& user_name,
		const std::vector<std::string_view>& idts_to_identify)
{
	const unsigned indexIdentity = 0;
	zmq::multipart_t toSend;
	flatbuffer_generator fg;

	auto[binary, size] = fg.generate_move_notif(user_name, pi);
	toSend.add({});
	toSend.addmem(binary, size);
	for (const auto& id : idts_to_identify) {
		toSend.at(indexIdentity).rebuild(id.data(), id.size());
		toSend.send(_routerPlayerConnection);
	}
}

uint
engine::retrieve_data_index(const auth_player& player)
{
	auto it = _auth_player_on_data_index.find(player);
	if (it == _auth_player_on_data_index.end()) {
		return NOT_AUTHENTICATED;
	}
	return _auth_player_on_data_index.at(player);
}

}
