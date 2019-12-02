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

#include <nlohmann/json.hpp>

#include <ArenaServerContext.hh>
#include <ConnectionHandler.hh>
#include <RandomGenerator.hh>
#include <fightingPit/contender/ContenderScripting.hh>
#include <fightingPit/contender/FightingContender.hh>
#include <fightingPit/FightingPitAnnouncer.hh>
#include <fightingPit/FightingPit.hh>

namespace fys::arena {
    using json = nlohmann::json;

    std::shared_ptr<FightingPit> FightingPitAnnouncer::buildFightingPit(const ArenaServerContext &ctx, ConnectionHandler &connectionHandler) const {
    }

    void FightingPitAnnouncer::generateContenders(const EncounterContext &ctx, const std::string &wsId) {
        const auto & range = ctx._rangeEncounterPerZone.at(wsId).at(static_cast<std::size_t>(_difficulty));
        unsigned numberContenders = fys::util::RandomGenerator::generateInRange(range.first, range.second);
        while (numberContenders--) {
            std::make_unique<ContenderScripting>(_)
            std::shared_ptr<FightingContender> addContender;
            _pitContenders.addContender(std::move(addContender));
        }
    }

    void FightingPitAnnouncer::generatePartyTeams() {
        
    }

    std::vector<std::unique_ptr<FightingContender> > FightingPitAnnouncer::generateScriptedContenders() {
        std::vector<std::unique_ptr<FightingContender> > resultContenders;

        return resultContenders;
    }

    std::vector<std::unique_ptr<FightingContender> > FightingPitAnnouncer::generateRandomContenders() {
        std::vector<std::unique_ptr<FightingContender> > resultContenders;

        return resultContenders;
    }



} // namespace fys::arena
