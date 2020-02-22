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

#ifndef FYS_ARENASERVERSERVICE_HH
#define FYS_ARENASERVERSERVICE_HH

#include <unordered_map>
#include <network/WorkerService.hh>
#include <ConnectionHandler.hh>
#include <CmlCopy.hh>

namespace fys::arena {

    class ArenaServerContext;

    /**
     * A player is awaited to generate an arena
     * AwaitingArena represent a player and the information relating to the arena that has to be generated
     */
    struct AwaitingArena {
        std::string _namePlayer;

        // fighting pit data
        bool _isAmbush;
        std::string _serverCode;
        uint _encounterId;
        FightingPit::Level _levelFightingPit;


    };

    /**
     * @brief Class managing an Arena Server.
     *
     * @details An arena server is a server handling multiple arena fighting pit instance at once.
     * In order to properly works, it need to work with an ArenaDispatcher, which is a proxy for the WorldServer to
     * communicate with Arena.
     * In case of a new encounter for a player, the following workflow apply:
     * - Player is moving / doing an action on the WorldMap (managed by a WorldServer) that is triggering a new encounter
     * - WorldServer send a message to a ArenaDispatcher containing basic configuration data to generate a fighting pit
     *   and authentication information, those are the following:
     *          - a generated token (used as authentication key)
     *          - the difficulty of the fight (configuration of the player)
     *          - the type of encounter (Random, Scripted etc...)
     *          - the id of encounter (if it is a scripted one)
     *          -
     */
    class ArenaServerService {
    public:
        explicit ArenaServerService(const ArenaServerContext &ctx);

        void runServerLoop() noexcept;

    private:
        void createNewFightingPit(AwaitingArena && arenaToCreate);
        void processMessage(std::string && idt, std::string && token, const zmq::message_t & content);

    private:
        std::reference_wrapper<const ArenaServerContext> _ctx;
        cache::CmlCopy      _cache;
        ConnectionHandler   _connectionHandler;
        WorkerService       _workerService;

        // map of token on awaiting arena
        std::unordered_map<std::string, AwaitingArena> _awaitingArena;
    };

}

#endif // !FYS_ARENASERVERSERVICE_HH
