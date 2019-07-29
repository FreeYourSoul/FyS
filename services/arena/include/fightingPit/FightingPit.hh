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
#include <fightingPit/contender/PitContenders.hh>
#include <fightingPit/contender/FightingContender.hh>
#include <fightingPit/FightingPitLayout.hh>
#include <fightingPit/team/AllyPartyTeams.hh>
#include <fightingPit/team/PartyTeam.hh>
#include <fightingPit/SideBattle.hh>

namespace fys::arena {


    /**
     * @brief A fighting pit represent a specific instance of a battle.
     *
     * A fightingpit is composed of:
     *  - a layout (mapping of how the monsters/players are spread on the board)
     *  - a PitContenders object managing all the monsters and their behaviours (scripting)
     *  - a AllyPartyTeams object managing the players, each client has a PartyTeam (multiple party teams possible for
     *    one AllyPartyTeams) and the PartyTeam is composed of TeamMember that are the actual characters spread on the
     *    gaming board.
     */
    class FightingPit {

    public:
        enum Level {
            EASY,
            MEDIUM,
            HARD
        };

        FightingPit(Level levelFigtingPit) : 
                _levelFightingPit(levelFightingPit), 
                _layout(_contenders, _partyTeams) 
        {}

        void startBattle();

        void addContender(std::shared_ptr<FightingContender> newContender);
        void addPartyTeam(std::unique_ptr<PartyTeam> newTeam);

    private:
        Level               _levelFightingPit;
        PitContenders       _contenders;
        AllyPartyTeams      _partyTeams;
        FightingPitLayout   _layout;

        std::vector<std::unique_ptr<SideBattle>> _sideBattles;

    };

}

#endif // !FYS_FIGHTINGPITINSTANCE_HH
