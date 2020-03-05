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

// forward declarations
namespace fys::arena {
    class FightingPit;
}

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
        explicit WorkerService() : _ctx(1), _workerRouter(_ctx, zmq::socket_type::router), _currentArenaId(0) {}

        /**
         * @brief Add a fighting pit to the worker service, adding this instance to the on-going/accessible fighting pit
         * of the arena server. Create an id for the newly created arena and set it to the fighting pit.
         *
         * @param fp FightingPit to add in the current WorkerService
         * @return newly added fighting pit id, or 0 (FightingPit::CREATION_ERROR) if an error occured
         * @todo : test that the arena id generation works properly
         */
        unsigned addFightingPit(std::unique_ptr<FightingPit> fp);

        /**
         *
         * @param userName player name to join the fightingpit
         * @param fightingPitId
         */
        void playerJoinFightingPit(std::string userName, unsigned fightingPitId);

        /**
         * Read on the router socket (connection with the players) and reply to them thanks to the zmq::router protocol
         * @tparam Lambda type following the signature => void (string, zmq::message_t)
         * @param HandlerAuth   Handler handler to call when receiving a message to authenticate an awaited player
         * @param HandlerInGame Handler handler to call when receiving a message to do a player action on a fightingPit
         */
        template <typename HandlerAuth, typename HandlerInGame>
        void pollAndProcessMessageFromPlayer(HandlerAuth && handlerAuth, HandlerInGame && handlerInGame) noexcept {
            //  Initialize poll set
            zmq::pollitem_t items[] = {
                    { _workerRouter, 0, ZMQ_POLLIN, 0 }
            };
            zmq::poll(&items[0], 1);
            if (static_cast<bool>(items[0].revents & ZMQ_POLLIN)) {
                zmq::multipart_t msg;
                if (!msg.recv(_workerRouter)) {
                    SPDLOG_ERROR("Error while reading on the arena worker listener socket");
                }
                else {
                    auto identity = msg.pop();
                    if ("auth" == msg.popstr())
                        std::forward<HandlerAuth>(handlerAuth)(std::move(identity), msg.pop());
                    else
                        std::forward<HandlerInGame>(handlerInGame)(std::move(identity), msg.pop());
                }
            }
        }

        /**
         * @brief Do elementary check (does the fighting arena exist), then forward the message to the fighting pit
         * @param fightingArenaId id of the arena to forward the message to
         * @param fightingMsg message to forward
         */
        void forwardMessageToFightingPit(unsigned fightingArenaId/* , const FightingMessage & fightingMsg*/);


    private:
        zmq::context_t      _ctx;
        zmq::socket_t       _workerRouter;
        unsigned            _currentArenaId;

        // map of client identifier to FightingArenaId
        std::unordered_map<unsigned, std::string> _idOnArenaId;
        std::unordered_map<unsigned, std::unique_ptr<FightingPit>> _arenaInstances;

    };

}

#endif // !FYS_WORKERSERVICE_HH
