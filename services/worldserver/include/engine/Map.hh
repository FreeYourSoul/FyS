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


#ifndef FYS_MAP_HH
#define FYS_MAP_HH

#include <optional>
#include <vector>
#include <string>

namespace fys::ws {

    class OverlapMaps {
        struct Overlap {
            bool operator()(double ix, double iy) {
                return check(ix, iy);
            }
            bool (*check)(double, double);
            double x;
            double y;
            std::string code;
        };

    public:
        std::vector<std::string_view> getOverlaps(double x, double y) {
            std::vector<std::string_view> ret(4);
            for (auto &over : _overlaps) {
                if (over(x, y))
                    ret.push_back(over.code);
            }
            return ret;
        }

    private:
        std::vector<Overlap> _overlaps;
    };


    class Map {

    public:
        Map(Map &&) noexcept = default;
        Map(const Map&) = delete;
        Map &operator=(const Map&) = delete;

        std::vector<std::string_view> getOverlapingMap(double x, double y);

    private:
        OverlapMaps _overlapMap;

    };

}

#endif //FYS_MAP_HH
