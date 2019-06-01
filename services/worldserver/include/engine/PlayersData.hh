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


#ifndef FYS_PLAYERSDATA_HH
#define FYS_PLAYERSDATA_HH

#include <unordered_map>
#include <utility>
#include <vector>
#include <array>

namespace fys::util {
    class Key;
}

namespace fys::ws {
    constexpr static double DEFAULT_DISTANCE = 30;

    struct Coordinate {
        double x;
        double y;
    };
//    using Coordinate = std::pair<double, double>;

    enum class PlayerStatus {
        FIGHTING,
        NORMAL
    };

    class PlayersData {

    public:
        explicit PlayersData(uint maxConnection = 1000) noexcept;


        Coordinate &getPlayerPosition(uint indexPlayer);
        /**
         * Retrieve the index corresponding to the given token
         *
         * @param token
         * @param idt
         * @return
         */
        uint getIndexAndUpdatePlayerConnection(const std::string &token, std::string idt);

        std::vector<std::string_view> getPlayerIdtsArroundPlayer(const std::string &token,
                double distance = DEFAULT_DISTANCE) const noexcept;
        std::vector<std::string_view> getPlayerIdtsArroundPos(Coordinate position,
                double distance = DEFAULT_DISTANCE) const noexcept;

    private:
        std::vector<Coordinate> _positions;
        std::vector<PlayerStatus> _status;
        std::vector<std::string> _identities;
        std::unordered_map<std::string, uint> _tokenToIndex;

    };

}


#endif //FYS_PLAYERSDATA_HH
