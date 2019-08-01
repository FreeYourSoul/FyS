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


#ifndef FYS_SIDEBATTLE_HH
#define FYS_SIDEBATTLE_HH

#include <functional>
#include <chrono>
#include <fightingPit/HexagonSide.hh>
#include <fightingPit/PriorityOrderList.hh>

namespace fys::arena {

    //forward declaration
    class PitContenders;
    class AllyPartyTeams;

    /**
     * @brief Represent a battle on a specific side of one of the 3 hexagons
     * 
     */
    class SideBattle {

    public:
        SideBattle(PitContenders &pitContenders, AllyPartyTeams &allyPartyTeams, HexagonSide::Orientation side) :
                _contenders(pitContenders),
                _partyTeams(allyPartyTeams),
                _side(side) 
        {}

    public:
        const data::PriorityElem &getCurrentParticipantTurn(const std::chrono::system_clock::time_point &now,
                                                            const std::chrono::milliseconds &timerInterlude);


    private:
        void endParticipantTurn(const std::chrono::milliseconds &timerInterlude) {
            _endCurrentTurn = std::chrono::system_clock::now() + timerInterlude;
            _priorityOrderList.getNext();
        }
 

    private:
        std::reference_wrapper<PitContenders>  _contenders; // possibly useless
        std::reference_wrapper<AllyPartyTeams> _partyTeams; // possibly useless
        PriorityOrderList        _priorityOrderList;
        HexagonSide::Orientation _side;

        std::chrono::system_clock::time_point  _endCurrentTurn;

    };

}


#endif //FYS_SIDEBATTLE_HH
