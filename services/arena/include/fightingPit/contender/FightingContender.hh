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


#ifndef FYS_FIGHTINGCONTENDER_HH
#define FYS_FIGHTINGCONTENDER_HH

#include <memory>
#include <fightingPit/data/CommonTypes.hh>
#include <fightingPit/contender/FightingContender.hh>
#include <fightingPit/contender/ContenderScripting.hh>
#include <fightingPit/HexagonSide.hh>

namespace fys::arena {

    class FightingContender {
    public:

        void moveContender(HexagonSide::Orientation destination, bool bypassCheck = false);
        /**
         * @param rightOrLeft(bool) move contender to the right if true, left otherwise
         */
        void moveContender(data::MoveDirection rightOrLeft);

        const data::Life &getLife() const { return _status.life; }
        

    private:
        FightingContender(std::unique_ptr<ContenderScripting> && contenderScripting);

    public:
        static FightingContender *makeFightingContender(std::unique_ptr<ContenderScripting> && contenderScripting) {
            return new FightingContender(std::move(contenderScripting));
        }

    private:
        std::unique_ptr<ContenderScripting> _contenderScripting;
        HexagonSide _side;
        data::Status _status;
    };

}

#endif // !FYS_FIGHTINGCONTENDER_HH
