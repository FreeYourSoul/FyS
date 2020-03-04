#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <StartupDispatcherCtx.hh>
#include <SimpleProxy.hh>

int main(int ac, char **av) {
    try {
        auto console = spdlog::stdout_color_mt("c");
        fys::StartupDispatcherCtx ctx(ac, av);
        SPDLOG_INFO(ctx.toString());
        fys::SimpleProxy dispatcher(ctx);

        dispatcher.startProxy();
    }
    catch (const std::exception &e) {
        SPDLOG_ERROR("Main caught an exception: {}", e.what());
    }
    return 0;
}
