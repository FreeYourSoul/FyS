#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <Dispatcher.hh>
#include "WorldServerContext.hh"

int main(int ac, char **av) {
    try {
        spdlog::set_pattern("[%D %H:%M:%S][ %@ ][%L]: %v");
        fys::ws::WorldServerCtx ctx(ac, av);
        SPDLOG_INFO(ctx.toString());
        fys::Dispatcher dispatcher(std::move(ctx));
        dispatcher.runDispatching();
    }
    catch (const std::exception &e) {
        SPDLOG_ERROR("Main caught an exception: {}", e.what());
    }
    return 0;
}
