#include <iostream>
#include "StartupDispatcherCtx.hh"

int main(int ac, char **av) {
    try {
        fys::Dispatcher dispatch(fys::StartupDispatcherCtx(ac, av));
    }
    catch (const std::exception &e) {
        std::cerr << "Main caught an exception: " << e.what() << "\n";
    }
    return 0;
}
