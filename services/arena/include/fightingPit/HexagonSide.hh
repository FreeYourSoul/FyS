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

#ifndef FYS_HEXAGONSIDE_HH
#define FYS_HEXAGONSIDE_HH

#include <utility>

namespace fys::arena {

    class HexagonSide {
    public:
        enum class Hexagone {
            A,
            B,
            C
        };

        enum class Orientation {
            A_N,
            A_S,
            A_NE,
            A_NW,
            A_SE,
            A_SW,
            B_N ,
            B_S ,
            B_NE,
            B_NW,
            B_SE,
            B_SW,
            C_N ,
            C_S ,
            C_NE,
            C_NW,
            C_SE,
            C_SW,
            NONE
        };

        std::pair<Hexagone, Orientation> operator->() const { return _sideOrientation; }

        bool move(const HexagonSide &destinationSide);
        bool moveRight();
        bool moveLeft();
        bool moveBack();

    private:
        Orientation findLeft() const;
        Orientation findRight() const;
        Orientation findBack() const;
        bool canMove(const HexagonSide &destinationSide) const;

    private:
        Hexagon     _hexagon;
        Orientation _side;
    };


    
} // namespace fys::arena



#endif // !FYS_HEXAGONSIDE_HH
