#include <Dispatcher.hh>


fys::Dispatcher::Dispatcher(fys::StartupDispatcherCtx &&ctx) :
 _connectionManager(1) {
    _connectionManager.setupConnectionManager(ctx);
}

void fys::Dispatcher::runDispatching() {

}