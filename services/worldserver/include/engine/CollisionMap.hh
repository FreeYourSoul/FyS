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


#ifndef FYS_COLLISIONMAP_HH
#define FYS_COLLISIONMAP_HH

#include <bitset>
#include <optional>
#include <vector>
#include <variant>
#include <utility>

namespace fys::ws {

    class WorldServerContext;
    class ConnectionHandler;

    class ProximityServer {
        struct ProximityServerAxis {
            double value;
            bool superiorTo;
        };

    public:
        constexpr bool isAtProximity(double axis) const noexcept {
            bool xReq = xAxisRequirement.has_value();
            bool yReq = yAxisRequirement.has_value();
            if (xReq)
                xReq = (xAxisRequirement->superiorTo) ? (axis > xAxisRequirement->value) : (axis < xAxisRequirement->value);
            if (yReq)
                yReq = (yAxisRequirement->superiorTo) ? (axis > yAxisRequirement->value) : (axis < yAxisRequirement->value);
            return xReq && yReq;
        }

        std::string code;
        std::optional<ProximityServerAxis> xAxisRequirement = std::nullopt;
        std::optional<ProximityServerAxis> yAxisRequirement = std::nullopt;
    };

    enum class eElementType {
        BLOCK,
        TRIGGER,
        NONE
    };

    class MapElement {

    public:
        constexpr void executePotentialTrigger(const std::string& token) const;
        inline bool canGoThrough(std::size_t level) const noexcept;

    private:
        inline bool canGoToLevel(std::size_t goLevel) const noexcept;

    private:
        std::bitset<4> _level;
        std::bitset<4> _changeLevel; // set on stairs to pass from a level to another
        eElementType _type = eElementType::NONE;
        std::variant<ConnectionHandler *, void *> _trigger;

    };

    class CollisionMap {

    public:
        CollisionMap(const WorldServerContext& ctx);
        CollisionMap(CollisionMap &&) noexcept = default;
        CollisionMap(const CollisionMap&) = delete;
        CollisionMap &operator=(const CollisionMap&) = delete;

        void buildMapFromTmx(const std::string &tmxMapPath);
        bool canMoveTo(double x, double y, std::size_t level) const noexcept;

    private:
        std::pair<double, double> _boundaryX;
        std::pair<double, double> _boundaryY;
        std::vector<ProximityServer> _serverProximity;
        std::vector<std::vector<MapElement>> _mapElems;

    };

}

#endif //FYS_COLLISIONMAP_HH
