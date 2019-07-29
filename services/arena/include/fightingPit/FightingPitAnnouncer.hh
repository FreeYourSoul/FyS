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


#ifndef FYS_FIGHTINGPITANNOUNCER_HH
#define FYS_FIGHTINGPITANNOUNCER_HH

#include <optional>
#include <memory>
#include <vector>
#include <fightingPit/contender/PitContenders.hh>
#include <fightingPit/team/AllyPartyTeams.hh>
#include "FightingPit.hh"

namespace fys::arena {

    /**
     * @brief Builder class to create the FightingPit. 
     *   - It generate the position of the Pit participants (contenders and team)
     *   - Define if the fight is an ambush or not.
     *   - Download the required scripts for the contenders (if not in the cache).
     *   - Download the required scripts for the team (if not in the cache)
     *   - Set the level of the fighting pit 
     * 
     */
    class FightingPitAnnouncer {

    public:
    enum EncounterType {
        RANDOM,
        QUEST,
        BOSS
    };

    public:
        FightingPitAnnouncer(EncounterType type, uint idEncounter = 0) : 
                _encounterType(type), 
                _idEncounter(idEncounter),
                _fightingPitLevel(FightingPit::Level::MEDIUM)
        {}

        FightingPit buildFightingPit() const;
        FightingPitAnnouncer &generateContenders(/* TODO: send ConnectionHandler for allowing scripts to send messages */);
        FightingPitAnnouncer &generatePartyTeams();

        FightingPitAnnouncer &setEncounterId(uint idEncounter) {
            _idEncounter = idEncounter; 
            return *this;
        }
        FightingPitAnnouncer &enforceAmbush(bool ambushEnforced) { 
            _isAmbushEnforced = ambushEnforced; 
            return *this;
        }

        FightingPit::Level getFightinPitLevel() const { return _fightingPitLevel; }

    private:

        void setupPartyTeam();

        // Generation of contenders

        std::vector<std::unique_ptr<FightingContender> > generateScriptedContenders();
        std::vector<std::unique_ptr<FightingContender> > generateRandomContenders();

        bool isBossEncounter() const { return _encounterType == EncounterType::BOSS; }
        bool isScriptedEncounter() const { return _encounterType != EncounterType::RANDOM; }

    private:
        EncounterType _encounterType;
        uint _idEncounter;
        FightingPit::Level  _fightingPitLevel;

        std::optional<bool> _isAmbushEnforced;

        PitContenders   _pitContenders;
        AllyPartyTeams  _partyTeams;

        bool _isFightingPitCorrupted = false;

        //TODO add the common cache retriever

    };

}

#endif // !FYS_FIGHTINGPITANNOUNCER_HH
