#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <zmq_addon.hpp>

#include <context/startup_dispatcher_ctx.hh>
#include <core/simple_proxy.hh>

int main(int ac, char** av) {
  try {
    spdlog::set_pattern("[%D %H:%M:%S][ %=30s:%# ][%L]: %v");
    fys::startup_dispatcher_ctx ctx(ac, av);
    int major, minor, patch;
    zmq_version(&major, &minor, &patch);
    SPDLOG_INFO("Version ZMQ : {}.{}.{}\n{}", major, minor, patch, ctx.to_string());

    fys::simple_proxy dispatcher(ctx);
    dispatcher.start_proxy();
  } catch (const std::exception& e) {
    SPDLOG_ERROR("Main caught an exception: {}", e.what());
  }
  return 0;
}
