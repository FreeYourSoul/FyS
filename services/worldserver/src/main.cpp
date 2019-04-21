//
// Created by FyS on 3/20/19.
//
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <WorldServerContext.hh>

int main(int ac, char **av) {
    try {
        auto console = spdlog::stdout_color_mt("c");
        fys::ws::WorldServerContext ctx(ac, av);
        spdlog::get("c")->info(ctx.toString());
    }
    catch (const std::exception &e) {
        spdlog::get("c")->error("Main caught an exception: {}", e.what());
    }
    return 0;
}