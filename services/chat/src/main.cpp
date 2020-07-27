/**
 *
 * Copyright (C) 2020 INAIT.SA - - All Rights Reserved
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 *
 * Proprietary and confidential
 *
 **/

#include <spdlog/spdlog.h>

int main(int ac, char **av) {
  try {
	spdlog::set_pattern("[%D %H:%M:%S][arena_server][ %22s:%-4# ][%L]: %v");
	fys::arena::arena_server_context ctx(ac, av);
	int major, minor, patch;
	zmq_version(&major, &minor, &patch);
	SPDLOG_INFO("Version ZMQ : {}.{}.{}\n{}", major, minor, patch, ctx.to_string());

	fys::arena::arena_server_service serverService(ctx);
	fys::arena::history_manager::activate_historic_manager(true);

	serverService.run_server_loop();
  }
  catch (const std::exception& e) {
	SPDLOG_ERROR("Main caught an exception: {}", e.what());
  }
  return 0;
}