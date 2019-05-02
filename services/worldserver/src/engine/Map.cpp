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


#include "engine/Map.hh"

// variant visitor trick
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace fys::ws {

// Map
bool Map::canMoveTo(double x, double y) const {
    if (x < 0.0 || x > _boundaryX || y < 0.0 || y > _boundaryY)
        return false;
    return _mapElems[static_cast<unsigned long>(x)][static_cast<unsigned long>(y)].canGoThrough();
}

std::vector<std::string_view> Map::getOverlapingMap(double x, double y) const {
    return _overlapMap.getOverlaps(x, y);
}


// Map Element
constexpr bool MapElement::canGoToLevel(std::size_t goLevel) const {
    return _changeLevel.test(goLevel);
}

constexpr bool MapElement::canGoThrough() const {
    return _type != eElementType::BLOCK;
}

constexpr void MapElement::executePotentialTrigger(const std::string &token) const {
    if (_type == eElementType::TRIGGER) {
        std::visit(overloaded {
            [](ConnectionHandler &trigger, const std::string &token) { // Player Trigger

            },
            [](void *trigger, const std::string &id) { // NPC Trigger

            }
        }, _trigger, token);
    }
}

}