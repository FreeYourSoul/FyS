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


#ifndef FYS_FIGHTINGPITLAYOUT_HH
#define FYS_FIGHTINGPITLAYOUT_HH

#include <functional>

namespace fys::arena {

    // forward declaration
    class AllyPartyTeams;
    class PitContenders;

    /**
     * @brief Layout of the fighting pit. Manage the positions of monsters and the players/monsters movements
     * 
     * The layout of the fighting pit can be taken as a draughtboard composed of 3 hexagons as shown as below
     *                        _____         
     *                       /     \        
     *                 _____/  B    \        18 possible playable sides
     *                /     \       /        6  possible ambush sides [A_NE, B_SW, A_SE, C_NW, C_N, B_S]
     *               /  A    \_____/         
     *               \       /     \
     *                \_____/  C    \
     *                      \       / 
     *                       \_____/ 
     *   
     * Each hexagon represent the monster position. The Player can have character of its team spread on the lines representing 
     * the sides of the hexagons.
     * Here is the hexagon at coordinate A, with its sides (where the characters can stands to attack the monster).
     *  
     *           A_N
     *           _____                N  = North
     * A_NW     /     \    A_NE       NE = North-East
     *         /  A    \              NW = North-West
     *         \       /              S  = South
     * A_SW     \_____/    A_SE       SE = South-East
     *                                SW = South-West
     *           A_S
     * 
     */
    class FightingPitLayout {

    public:
        FightingPitLayout(PitContenders &contenders, AllyPartyTeams &partyTeams) : _contenders(contenders), _partyTeams(partyTeams)
        {}

        void contenderMove();
        void characterMove();

    private:
        std::reference_wrapper<PitContenders>       _contenders;
        std::reference_wrapper<AllyPartyTeams>      _partyTeams;

    };

}

#endif // !FYS_FIGHTINGPITLAYOUT_HH
