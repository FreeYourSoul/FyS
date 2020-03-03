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

    public:
        // used as a wrong id when a fighting pit is wrongly generated
        static constexpr unsigned CREATION_ERROR = 0;

        enum Level : uint {
            EASY    = 0,
            MEDIUM  = 1,
            HARD    = 2
        };

        enum Ending {
            ON_HOLD,        // on hold mechanism for joining raid
            CONTENDER_WIN,
            ALLY_WIN,
            NOT_FINISHED    // on going
        };

        struct AuthenticatedPlayer {
            std::string name;
            std::string token;
        };

        explicit FightingPit(std::string creatorUserName, Level levelFightingPit);

        const std::unique_ptr<chaiscript::ChaiScript> &getChaiPtr() const { return _chaiPtr; }

        void startBattle();
        void addAuthenticatedUser(std::string userName, std::string userToken);
        void addContender(std::shared_ptr<FightingContender> newContender) { _contenders.addContender(std::move(newContender)); }
        void addPartyTeam(std::unique_ptr<PartyTeam> newTeam) { _partyTeams.addPartyTeam(std::move(newTeam)); }

        const std::string &getCreatorUserName() const { return _creatorUserName; }
        const PitContenders &getPitContenders() const { return _contenders; }
        const AllyPartyTeams &getPartyTeam() const { return _partyTeams; }
        const FightingPitLayout &getLayout() const { return _layout; }

        unsigned getArenaId() const { return _arenaId; }
        void setArenaId(unsigned arenaId) { _arenaId = arenaId; }

    private:
        void readInputAndAppendPendingActions();

    private:
        Ending              _end = ON_HOLD;
        Level               _levelFightingPit;
        // TODO : add connection handler to WorkerService
        PitContenders       _contenders;
        AllyPartyTeams      _partyTeams;
        FightingPitLayout   _layout;
        std::string         _creatorUserName;

        unsigned            _arenaId;
        std::vector<AuthenticatedPlayer> _authenticatedPlayers;

        std::unique_ptr<chaiscript::ChaiScript> _chaiPtr;
        std::vector<std::unique_ptr<SideBattle>> _sideBattles;

    };

}

#endif // !FYS_FIGHTINGPITINSTANCE_HH
