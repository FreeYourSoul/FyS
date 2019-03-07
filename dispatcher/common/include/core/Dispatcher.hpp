#ifndef FYS_DISPATCHER_HH_
#define FYS_DISPATCHER_HH_

#include <StartupDispatcherCtx.hh>

namespace fys
{
    class Dispatcher {
        public:
            Dispatcher(fys::StartupDispatcherCtx &&ctx);

            void runDispatching();

        private:
            fys::network::DispatcherConnectionManager _connectionManager;
    };
} 
#endif // !FYS_DISPATCHER_HH_
