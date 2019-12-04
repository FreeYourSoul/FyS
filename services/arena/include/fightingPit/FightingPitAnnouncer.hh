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
#include <fightingPit/FightingPit.hh>

// forward declarations
namespace fys::cache {
    class Cml;
}
namespace fys::arena {
    class EncounterContext;
}

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
            SCRIPTED
        };

    public:
        explicit FightingPitAnnouncer(cache::Cml &cml) : _cache(cml) {}

        [[nodiscard]] std::shared_ptr<FightingPit>
        buildFightingPit(const ArenaServerContext &ctx, ConnectionHandler &connectionHandler, const std::string &wsId);

        void setEncounterType(EncounterType encounterType) {
            if (encounterType == RANDOM)
                _idEncounter = 0;
            _encounterType = encounterType;
        }

        void setEncounterId(uint idEncounter) {
            if (_encounterType == RANDOM && !idEncounter)
                _encounterType = SCRIPTED;
            _idEncounter = idEncounter;
        }

        void enforceAmbush(bool ambushEnforced) { _isAmbushEnforced = ambushEnforced; }

        void setDifficulty(FightingPit::Level level) { _difficulty = level; }

    private:
        void generateContenders(const FightingPit &fp, const EncounterContext &ctx, const std::string &wsId);

        void generatePartyTeams();

        [[nodiscard]] bool isScriptedEncounter() const { return _encounterType != EncounterType::RANDOM; }
        [[nodiscard]] bool isRandomEncounter() const { return _encounterType == EncounterType::RANDOM; }

    private:
        cache::Cml &_cache;

        /**
         *   range           desc
         *     0    : random encounter generation
         * [1001-*] : globally defined encounters
         * [1-1000] : zone defined encounters
         */
        uint _idEncounter = 0;
        FightingPit::Level _difficulty = FightingPit::Level::MEDIUM;
        EncounterType _encounterType = EncounterType::RANDOM;

        std::optional<bool> _isAmbushEnforced;

        PitContenders _pitContenders;
        AllyPartyTeams _partyTeams;

        //TODO add the common cache retriever

    };

}

#endif // !FYS_FIGHTINGPITANNOUNCER_HH
