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


#ifndef FYS_PITCONTENDERS_HH
#define FYS_PITCONTENDERS_HH

#include <memory>
#include <vector>
#include <functional>
#include <fightingPit/data/CommonTypes.hh>
#include <fightingPit/HexagonSide.hh>

namespace fys::arena {

    template <typename T>
    using ComparatorSelection = std::function<bool(std::shared_ptr<T>, std::shared_ptr<T>)>;

    //forward declarations
    class FightingContender;
    class AllyPartyTeams;

    class PitContenders {

    public:
        PitContenders() = default;
        PitContenders(const PitContenders& other) {}
        [[nodiscard]] std::vector<std::shared_ptr<FightingContender>> getContenderOnSide(HexagonSide::Orientation side) const;
        [[nodiscard]] std::vector<std::shared_ptr<FightingContender>> getChangingSideContenders() const;

        // scripting utility
        [[nodiscard]] std::shared_ptr<FightingContender>
            selectSuitableContender(ComparatorSelection<FightingContender> comp) const;
        [[nodiscard]] std::shared_ptr<FightingContender>
            selectSuitableContenderOnSide(HexagonSide::Orientation side, ComparatorSelection<FightingContender> comp) const;
        [[nodiscard]] std::shared_ptr<FightingContender>
            selectSuitableContenderAlive(ComparatorSelection<FightingContender> comp) const;
        [[nodiscard]] std::shared_ptr<FightingContender>
            selectSuitableContenderOnSideAlive(HexagonSide::Orientation side, ComparatorSelection<FightingContender> comp) const;
        [[nodiscard]]std::shared_ptr<FightingContender>
            getFightingContender(std::size_t pos) const { return _contenders.at(pos); }
        [[nodiscard]]
            std::size_t getNumberContender() const { return _contenders.size(); }
        [[nodiscard]]
            const std::vector<std::shared_ptr<FightingContender> > &getContenders() const { return _contenders; }

        void executeContenderAction(const data::PriorityElem &contender);
        void addContender(const std::shared_ptr<FightingContender> & contender);
        unsigned contenderOnSide(HexagonSide::Orientation side) const;

    private:
        std::vector<std::shared_ptr<FightingContender> > _contenders;

        /**
         * Flags determining which contenders are going to move from one side to another
         * (only _contenders having this flag (index equivalent) set to true have their position refreshed
         */
        std::vector<bool> _changeSideFlags;

    };

}

#endif // !FYS_PITCONTENDERS_HH
