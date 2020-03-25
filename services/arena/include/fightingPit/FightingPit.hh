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


#ifndef FYS_FIGHTINGPITINSTANCE_HH
#define FYS_FIGHTINGPITINSTANCE_HH

#include <vector>
#include <optional>
#include <chrono>

#include <chaiscript/chaiscript.hpp>

#include <fightingPit/contender/PitContenders.hh>
#include <fightingPit/FightingPitLayout.hh>
#include <fightingPit/SideBattle.hh>
#include <fightingPit/team/AllyPartyTeams.hh>
#include <ChaiRegister.hh>

// forward declarations
namespace fys::arena {
    class FightingContender;
    class PartyTeam;
}

namespace fys::arena {
    class FightingPitAnnouncer;

    /**
     * @brief A fighting pit represent a specific instance of a battle.
     *
     * A fightingpit is composed of:
     *  - a layout (mapping of how the monsters/players are spread on the board)
     *  - a PitContenders object managing all the monsters and their behaviours (scripting)
     *  - a AllyPartyTeams object managing the players, each client has a PartyTeam (multiple party teams possible for
     *    one AllyPartyTeams) and the PartyTeam is composed of TeamMember that are the actual characters spread on the
     *    gaming board.
     *  - The 18 instances of SideBattle
     * 
     * @see SideBattle
     */
    class FightingPit {

        friend class FightingPitAnnouncer;

        enum Ending {
            ON_HOLD,        // on hold mechanism for joining raid
            NOT_FINISHED,    // on going
            CONTENDER_WIN,
            ALLY_WIN
        };

        struct AuthenticatedPlayer {
            std::string name;
            std::string token;
        };

    public:
        // used as a wrong id when a fighting pit is wrongly generated
        static constexpr unsigned CREATION_ERROR = 0;

        enum Level : uint {
            EASY    = 0,
            MEDIUM  = 1,
            HARD    = 2,
            NONE
        };

        explicit FightingPit(std::string creatorUserName, Level levelFightingPit);

        /**
         *
         * @param now
         */
        void continueBattle(const std::chrono::system_clock::time_point & now);

        /**
         *
         * @param userName
         * @param nameMember
         */
        void forwardMessageToTeamMember(const std::string & userName, const std::string & nameMember);

        /**
         * Add an authenticated player in the fighting pit, the player authentication is not verified an thus must be
         * verified beforehand.
         * @param userName unique name of the player to add
         * @param userToken token of the player to add
         */
        void addAuthenticatedUser(std::string userName, std::string userToken);
        /**
         * Check if given player (defined by name/token) is authenticated
         * @param name unique name of the player to check
         * @param token token of the player to check
         * @return true if the player is authenticated, false otherwise
         */
        bool isPlayerParticipant(const std::string & name, const std::string & token) const;

        /**
         * Used to add a new party team (incoming player in the fighting pit)
         * @param pt party team to add
         */
        void addPartyTeam(std::unique_ptr<PartyTeam> pt);

        const std::unique_ptr<chaiscript::ChaiScript> &getChaiPtr() const { return _chaiPtr; }
        void setArenaId(unsigned arenaId) { _arenaId = arenaId; }

    private:
        /**
         * Check if the fight has started, if it has check if the fight is finished (winner determined)
         * If ally wins, a generation of the loot is made and returned to the clients
         * If enemy wins, a notification is sent to players
         * @return true if the fight has been terminated, false otherwise
         */
        bool checkEndStatusFightingPit();

        void addContender(const std::shared_ptr<FightingContender> & fc) { _contenders.addContender(fc); }

        void initializePartyTeam(AllyPartyTeams && allyPartyTeams);
        void initializeSideBattles();
        void initializePriorityListInSidesBattle();

    private:
        Ending                      _end = ON_HOLD;
        Level                       _levelFightingPit;
        std::chrono::milliseconds   _timeInterlude;
        PitContenders               _contenders;
        AllyPartyTeams              _partyTeams;

        // mapping of the contenders/NPC with the layout of the FightingPit
        FightingPitLayout   _layoutMapping;

        std::string         _creatorUserName; // useless ?
        unsigned            _arenaId;         // useless ?

        std::vector<AuthenticatedPlayer> _authenticatedPlayers;

        std::unique_ptr<chaiscript::ChaiScript>     _chaiPtr;
        std::vector<SideBattle>                     _sideBattles;

    };

}

#endif // !FYS_FIGHTINGPITINSTANCE_HH
