#include <iostream>
#include <StartupDispatcherCtx.hh>
#include <Dispatcher.hh>

int main(int ac, char **av) {
    try {
        fys::StartupDispatcherCtx ctx(ac, av);
        fys::Dispatcher dispatch();

    }
    catch (const std::exception &e) {
        std::cerr << "Main caught an exception: " << e.what() << "\n";
    }
    return 0;
}
