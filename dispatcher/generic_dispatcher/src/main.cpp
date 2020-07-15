#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <zmq_addon.hpp>

#include <core/SimpleProxy.hh>
#include <context/StartupDispatcherCtx.hh>

int main(int ac, char **av) {
    try {
        spdlog::set_pattern("[%D %H:%M:%S][ %=30s:%# ][%L]: %v");
        fys::StartupDispatcherCtx ctx(ac, av);
        int major, minor, patch;
        zmq_version(&major, &minor, &patch);
        SPDLOG_INFO("Version ZMQ : {}.{}.{}\n{}", major, minor, patch, ctx.toString());

        fys::SimpleProxy dispatcher(ctx);
        dispatcher.startProxy();
    }
    catch (const std::exception &e) {
        SPDLOG_ERROR("Main caught an exception: {}", e.what());
    }
    return 0;
}
