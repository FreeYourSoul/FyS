// MIT License
//
// Copyright (c) 2019 Quentin Balland
// Repository : https://github.com/FreeYourSoul/FyS
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
//         of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
//         to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//         copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
//         copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//         AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef FYS_WORKERSERVICE_HH
#define FYS_WORKERSERVICE_HH

#include <zmq_addon.hpp>
#include <fightingPit/FightingPitAnnouncer.hh>

 namespace fys::arena {
 
     /**
      * @brief Manage the fighting pits : 
      *   - generate a fighting pit
      *   - assign an ID to it
      *   - assign a dedicated thread to it
      * 
      * Fighting pit are created by the WorkerService using the FightingPitAnnouncer as builder in order to 
      * ensure that the fightingpit is ready to host a battle, and then start the fighting pit in a dedicated
      * thread.
      * 
      * Each fighting pit will contains a DEALER socket that will be used to communicate with the WorkerService.
      * A custom zmq identifier will be provided by the WorkerService in order to be able to target a specific
      * FightingPit for specific players (the map _idOnArenaId being used to register which player is in which
      * FightingPit.
      * 
      * @see FightingPitAnnouncer
      * @see FightingPit
      */
    class WorkerService {
    public:
        void generateFightingPit(FightingPitAnnouncer announcer);

        void forwardMessageToFightingPit(const std::string &fightingArenaId/* , FightingMessage*/);


    private:
        zmq::socket_t       _workerRouter;

        // map of client identitier to FightingArenaId
        std::unordered_map<std::string, std::string> _idOnArenaId;
    };

}

#endif // !FYS_WORKERSERVICE_HH
