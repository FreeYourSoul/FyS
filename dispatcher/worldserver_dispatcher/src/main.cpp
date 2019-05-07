#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <Dispatcher.hh>
#include "WorldServerContext.hh"

int main(int ac, char **av) {
    try {
        auto console = spdlog::stdout_color_mt("c");
        fys::ws::WorldServerCtx ctx(ac, av);
        spdlog::get("c")->info(ctx.toString());
        fys::Dispatcher dispatcher(std::move(ctx));
        dispatcher.runDispatching();
    }
    catch (const std::exception &e) {
        spdlog::get("c")->error("Main caught an exception: {}", e.what());
    }
    return 0;
}
