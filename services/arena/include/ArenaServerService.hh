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
     * An arena is awaited to be generated
     * AwaitingArena represent a player that is the trigger of a new fighting pit, and the information relating to the
     * arena that has to be generated.
     */
    struct AwaitingArena {
        std::string _namePlayer;
        std::string _token;

        // fighting pit data
        std::string _serverCode;
        bool _isAmbush;
        uint _encounterId;
        FightingPit::Level _levelFightingPit;


    };

    /**
     * @brief Class managing an Arena Server.
     *<br/>
     * An arena server is a server handling multiple arena fighting pit instance at once.
     * In order to properly works, it need to work with an ArenaDispatcher, which is a proxy for the WorldServer to
     * communicate with Arena.
     *
     * @createNewFightingPit In case of a new encounter for a player, the following workflow apply:
     * 1 - Player is moving / doing an action on the WorldMap (managed by a WorldServer) that is triggering a new
     *     encounter.
     *     <br/><br/>
     * 2 - WorldServer send a message to a ArenaDispatcher containing configuration data to generate a fighting pit and
     *     authentication information, those are the following:<br/>
     *          2.a - a generated token (used as authentication key)<br/>
     *          2.b - the difficulty of the fight (configuration of the player)<br/>
     *          2.c - the id of encounter; if equal to 0, it is a random encounter. scripted id otherwise.<br/>
     * 3 - The Dispatcher forward the message to an ArenaServer that is going to register the incoming player that will
     *     initiate the creation of the fighting pit.
     *     <br/><br/>
     * 4 - The Arena server reply to the dispatcher (that will reply to the WorldServer), the message will return to the
     *     player, message containing the ip (connection string) of the ArenaServer that will create the fighting pit.
     *     <br/><br/>
     * 5 - The Player can then directly connect to the ArenaServer using the token to validate the authentication, The
     *     Arena service validate the connection, and use the data (difficulty ect..) in order to generate the
     *     fighting pit (with an id unique). When the fight is setup, the initial handshake with the client contains the
     *     id of the fighting pit in order to enable other player to join.
     *     <br/>
     *
     * @joinFightingPit Another player than the creator can join an existing arena by specifying its fighting pit id to
     * the WorldServer, which will check if the position of the joining player is close enough of the creator of the
     * fighting pit. The ArenaServer is then just going to check the existence of the id and will add the player into
     * the fight.
     *
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
