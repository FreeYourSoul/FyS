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


#ifndef FYS_COMMONDATA_HH
#define FYS_COMMONDATA_HH

#include <vector>

namespace fys::arena::data
{
    static constexpr bool CONTENDER = true;
    static constexpr bool PARTY_MEMBER = false;

    struct PriorityElem { // Improve with strong typing on ID/SPEED
        PriorityElem() = default;
        PriorityElem(uint aid, int aspeed, bool aisContender) : id(aid), speed(aspeed), isContender(aisContender) {}

        uint id;
        int  speed;
        bool isContender;

        PriorityElem &operator-(const PriorityElem &other) {
            speed -= other.speed;
            return *this;
        }

        bool operator<(const PriorityElem &other) const {
            return speed < other.speed;
        }

        bool operator==(const PriorityElem &other) const {
            return id == other.id && isContender == other.isContender;
        }

    };

    struct Life {
        uint current = 0;
        uint total   = 0;

        [[nodiscard]] bool isDead() const { return current == 0; }
    };

    struct MagicPoint {
        uint current = 0;
        uint total   = 0;
    };

    using AlterationId = uint;

    struct Status {
        Life life;
        MagicPoint magicPoint;
        uint initialSpeed;
        std::vector<AlterationId> alterations;
    };

    enum MoveDirection {
        BACK,
        RIGHT,
        LEFT
    };

    enum Targeting {
        SELF,
        ENNEMY,
        ALLY,
        ENNEMIES,
        ALLIES,
        ALLY_AND_ENNEMY,
        ALLY_OR_ENNEMY,
        SIDE
    };
    
} // namespace fys::arena::data

#endif // !FYS_COMMONDATA_HH
