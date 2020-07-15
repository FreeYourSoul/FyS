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


#ifndef FYS_ONLINE_WORLD_POPULATOR_HH
#define FYS_ONLINE_WORLD_POPULATOR_HH

namespace fys::ws {
class world_server_context;
class script_engine;
class collision_map;
class engine;
class spawning_point;
}

namespace fys::ws {

static constexpr std::chrono::milliseconds TIMING_MOVE_INTERVAL(16);


/**
 * WorldPopulator is used in order to get the data required to populate the section of the map handled by
 * the World Server Service. It request a database in order to:
 * - download NPC scripts, and trigger, to be used by Cml (Cache Manager Library) afterwards
 * - retrieve the NPC position (with their associated script id)
 * - retrieve the Monster spawn position (with their associated script id)
 * - get the trigger points (with their associated script id) of the map and set them in the map
 */
class world_populator {

public:
	~world_populator();

	[[nodiscard]] std::shared_ptr<engine>
	build_world_server_engine();

	// Builder methods
	world_populator& populate_map(const world_server_context& ctx);
	world_populator& populate_script_engine(const world_server_context& ctx);
	world_populator& set_connection_string(std::string connectionString);
	world_populator& set_interval_movement(std::chrono::system_clock::duration interval_movement);

	// Getters for testing
	const std::vector<spawning_point> &get_spawning_point() const;
	script_engine &get_script_engine() const { return *_script_engine; }

private:
	void register_common_lua_engine(const std::string& to_lua_init_file);
	void generate_spawning_points(const std::string& spawning_point_config_path, const std::string& base_path);

	[[nodiscard]] std::string
	get_path_from_key(std::string base, std::string key)
	{
		std::replace(key.begin(), key.end(), ':', '/');
		return base.append("/").append(std::move(key));
	}

	[[nodiscard]] std::string
	get_sp_namespace_from_key(const std::string& key)
	{
		auto startSeparator = key.find_last_of(':');
		auto endSeparator = key.find_last_of('.');

		if (startSeparator != std::string::npos && endSeparator != std::string::npos) {
			// if a ':' and a '.' are found
			return key.substr(startSeparator + 1, endSeparator - startSeparator - 1);
		}
		else if (startSeparator == std::string::npos && endSeparator != std::string::npos) {
			// if a ':' is not found but not a '.' is found
			return key.substr(0, endSeparator);
		}
		else if (startSeparator != std::string::npos && endSeparator == std::string::npos) {
			// if a ':' is found but not a '.' is not found
			return key.substr(startSeparator + 1);
		}
		return key;
	}

private:
	std::string _connection_string;
	std::chrono::system_clock::duration _interval_movement = TIMING_MOVE_INTERVAL;

	std::shared_ptr<collision_map> _map;
	std::unique_ptr<script_engine> _script_engine;

};

}

#endif //FYS_ONLINE_WORLD_POPULATOR_HH
