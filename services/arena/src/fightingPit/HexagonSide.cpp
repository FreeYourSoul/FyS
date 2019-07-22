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

#include <fightingpit/HexagonSide.hh>

namespace
{
    struct HexaA {};
    struct HexaB {};
    struct HexaC {};
    struct South;
    struct SouthEast;
    struct SouthWest;
    struct North;
    struct NorthEast;
    struct NorthWest;

    template<typename ID, typename Orient>
    struct HexaSideOrient {
    };

    template<>
    struct HexaSideOrient<HexaA, North> {
        static bool isMoveable(HexagoneSide::Orientation dest) { return dest == h::A_NE || dest == h::A_NW; }
    };

    template<>
    struct HexaSideOrient<HexaA, South> {
        static bool isMoveable(HexagoneSide::Orientation dest) { return dest == h::A_SE || dest == h::A_SW || dest == h::C_NW || dest == h::C_SW; } 
    };

    template<>
    struct HexaSideOrient<HexaA, NorthEast> {
        static bool isMoveable(HexagoneSide::Orientation dest) { 
            return 
                dest == h::A_N || dest == h::A_SE || 
                dest == h::B_S || dest == h::B_NW || dest == h::B_SW || 
                dest == h::C_N || dest == h::C_NW; 
        } 
    };

    template<>
    struct HexaSide<HexaA, NorthWest> {
        static bool isMoveable(HexagoneSide::Orientation dest) { return dest == h::A_N || dest == h::A_SW; } 
    };

    template<> // A_SE
    struct HexaSide<HexaA, SouthEast> {
        static bool isMoveable(HexagoneSide::Orientation dest) { 
            return 
                dest == h::A_NE || dest == h::A_S || 
                dest == h::B_S || dest == h::B_SW || 
                dest == h::C_N || dest == h::C_NW || dest == h::C_SW; 
        } 
    };

    template<> // A_SW
    struct HexaSide<HexaA, SouthWest> {
        static bool isMoveable(HexagoneSide::Orientation dest) { return dest == h::A_NW || dest == h::A_S; } 
    };

    template<typename Hexa>
    struct HexaSide;

    template <>
    struct HexaSide<HexaA> {
        static bool isMoveable(HexagoneSide::Orientation origin, HexagoneSide::Orientation dest) { 
            if (*origin == *destination)
                return false;
            if (dest == HexagonSide::Orientation::A_S)
                return HexaSideOrient<HexaA, South>::isMoveable(dest);
            else if (dest == HexagonSide::Orientation::A_SE)
                return HexaSideOrient<HexaA, SouthEast>::isMoveable(dest);
            else if (dest == HexagonSide::Orientation::A_SW)
                return HexaSideOrient<HexaA, SouthWest>::isMoveable(dest);
            if (dest == HexagonSide::Orientation::A_N)
                return HexaSideOrient<HexaA, North>::isMoveable(dest);
            else if (dest == HexagonSide::Orientation::A_NE)
                return HexaSideOrient<HexaA, NorthEast>::isMoveable(dest);
            else if (dest == HexagonSide::Orientation::A_NW)
                return HexaSideOrient<HexaA, NorthWest>::isMoveable(dest);
            return false;
        } 
    };

    template <>
    struct HexaSide<HexaB> {
        static bool isMoveable(HexagoneSide::Orientation origin, HexagoneSide::Orientation dest) { 
            if (*origin == *destination)
                return false;
            if (dest == HexagonSide::Orientation::A_S)
                return HexaSideOrient<HexaB, South>::isMoveable(dest);
            else if (dest == HexagonSide::Orientation::B_SE)
                return HexaSideOrient<HexaB, SouthEast>::isMoveable(dest);
            else if (dest == HexagonSide::Orientation::B_SW)
                return HexaSideOrient<HexaB, SouthWest>::isMoveable(dest);
            if (dest == HexagonSide::Orientation::B_N)
                return HexaSideOrient<HexaB, North>::isMoveable(dest);
            else if (dest == HexagonSide::Orientation::B_NE)
                return HexaSideOrient<HexaB, NorthEast>::isMoveable(dest);
            else if (dest == HexagonSide::Orientation::B_NW)
                return HexaSideOrient<HexaB, NorthWest>::isMoveable(dest);
            return false;
        } 
    };

    template <>
    struct HexaSide<HexaC> {
        static bool isMoveable(HexagoneSide::Orientation origin, HexagoneSide::Orientation dest) { 
            if (*origin == *destination)
                return false;
            if (dest == HexagonSide::Orientation::C_S)
                return HexaSideOrient<HexaC, South>::isMoveable(dest);
            else if (dest == HexagonSide::Orientation::C_SE)
                return HexaSideOrient<HexaC, SouthEast>::isMoveable(dest);
            else if (dest == HexagonSide::Orientation::C_SW)
                return HexaSideOrient<HexaC, SouthWest>::isMoveable(dest);
            if (dest == HexagonSide::Orientation::C_N)
                return HexaSideOrient<HexaC, North>::isMoveable(dest);
            else if (dest == HexagonSide::Orientation::C_NE)
                return HexaSideOrient<HexaC, NorthEast>::isMoveable(dest);
            else if (dest == HexagonSide::Orientation::C_NW)
                return HexaSideOrient<HexaC, NorthWest>::isMoveable(dest);
            return false;
        } 
    };

} // namespace anonyme


namespace fys::arena {
    
    
    bool HexagonSide::move(const HexagonSide &destinationSide) {
        return true;
    }

    bool HexagonSide::moveLeft() {
        return move(findLeft());
    }

    bool HexagonSide::moveRight() {
        return move(findRight());
    }
    
    bool HexagonSide::moveBack() {
        return move(findBack());
    }

    // private API

    HexagonSide::Orientation findLeft() const {
        return HexagonSide::Orientation::NONE;
    }

    HexagonSide::Orientation findRight() const {
        return HexagonSide::Orientation::NONE;
    }

    HexagonSide::Orientation findBack() const {
        return HexagonSide::Orientation::NONE;
    }

    bool HexagonSide::canMove(const HexagonSide &destinationSide) const {
        if (_hexagon == Hexagone::A)
            return HexaSide<HexaA>::isMoveable(_side, destinationSide);
        // else if (_hexagon == Hexagone::B)
        //     return HexaSide<HexaB>::isMoveable(_side, destinationSide);
        // else if (_hexagon == Hexagone::C)
        //     return HexaSide<HexaC>::isMoveable(_side, destinationSide);
        return false;
    }


} // namespace fys::arena

