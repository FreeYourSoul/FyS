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
#include <fightingPit/HexagonSide.hh>

namespace fys::arena {

    //forward declarations
    class FightingContender;

    class PitContenders {

    public:
        std::vector<std::shared_ptr<FightingContender>> getContenderOnSide(HexagonSide::Orientation side);
        std::vector<std::shared_ptr<FightingContender>> getChangingSideContenders();

        void addContender(std::shared_ptr<FightingContender> contender) {
            _contenders.emplace_back(std::move(contender));
            _changeSideFlags.emplace_back(true);
        }

    private:
        std::vector<std::shared_ptr<FightingContender> > _contenders;
        std::vector<bool> _changeSideFlags;

    };

}

#endif // !FYS_PITCONTENDERS_HH
