// MIT License
//
// Copyright (c) 2021-2022 Quentin Balland
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

#include <random>

#include <spdlog/spdlog.h>
//#include <cmrc/cmrc.hpp>

#include <zmq_addon.hpp>

#include <arena_server_context.hh>
#include <arena_server_service.hh>
#include <history_manager.hh>

//CMRC_DECLARE(resource);

int main(int ac, char** av) {
  try {
    spdlog::set_pattern("[%D %H:%M:%S][arena_server][ %25s:%-4# ][%L]: %v");
    fys::arena::arena_server_context ctx(ac, av);
    int major, minor, patch;
    zmq_version(&major, &minor, &patch);
    spdlog::info("Version ZMQ : {}.{}.{}\n{}", major, minor, patch, ctx.to_string());

    fys::arena::arena_server_service serverService(ctx);
    fys::arena::history_manager::activate_historic_manager(true);
    fys::arena::history_manager::set_save_path(ctx.get_history_saving_folder());

    serverService.run_server_loop();
  } catch (const std::exception& e) {
    SPDLOG_ERROR("Main caught an exception: {}", e.what());
  }
  return 0;
}
