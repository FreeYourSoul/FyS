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
#include <spdlog/sinks/stdout_color_sinks.h>
#include <engine/world_populator.hh>
#include <world_server_context.hh>
#include <world_server_service.hh>
#include <key.hh>

int
main(int ac, char** av)
{
	try {
		spdlog::set_pattern("[%D %H:%M:%S][ %22s:%-4# ][%L]: %v");
		fys::ws::world_server_context ctx(ac, av);
		int major, minor, patch;
		zmq_version(&major, &minor, &patch);
		SPDLOG_INFO("Version ZMQ : {}.{}.{}\n{}", major, minor, patch, ctx.to_string());

		fys::ws::world_server_service serverService(ctx,
				std::move(*fys::ws::world_populator()
						.set_interval_movement(fys::ws::TIMING_MOVE_INTERVAL)
						.set_connection_string(ctx.get_player_connection_str())
						.populate_map(ctx)
						.populate_script_engine(ctx)
						.build_world_server_engine())
		);
		serverService.run_server_loop();
	}
	catch (const std::exception& e) {
		SPDLOG_ERROR("Main caught an exception: {}", e.what());
	}
	return 0;
}