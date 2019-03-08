//
// Created by FyS on 3/9/19.
//

#ifndef PROJECT_WORLDSERVERDISPATCHERHANDLER_HH
#define PROJECT_WORLDSERVERDISPATCHERHANDLER_HH

namespace fys::worldserver {

    class WorldServerDispatcherHandler {
    protected:
        /**
         * @brief This method is processing the inputMessage and dispatch it appropriately among the peers connected to
         * the DEALER socket
         */
        void processInputMessage();

        /**
         * @brief This method is dispatching the message
         */
        void processClusterMessage();

    };

}


#endif //PROJECT_WORLDSERVERDISPATCHERHANDLER_HH
