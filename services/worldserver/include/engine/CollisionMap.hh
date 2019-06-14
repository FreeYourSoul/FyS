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

#include <algorithm>
#include <bitset>
#include <optional>
#include <vector>
#include <variant>
#include <utility>
#include <tmxlite/Types.hpp>
#include "PlayersData.hh"

namespace fys::map::algo {

    /**
     * Check if the given object layer is used for collision
     * @tparam TmxLayer tmx::GroupObjects layer
     * @param layer perfect forwarding object
     * @return true if the given group object layer is used for collision, false otherwise
     */
    template <typename TmxLayer>
    bool isCollisionLayer(TmxLayer && layer) {
        return std::any_of(layer.getProperties().begin(), layer.getProperties().end(), [](const auto &prop){
            return prop.getName().find("collision") != std::string::npos;
        });
    }

    /**
     * Check if the given object layer is used for any trigger
     * @tparam TmxLayer tmx::GroupObjects layer
     * @param layer perfect forwarding object
     * @return true if the given group object layer is used for any trigger, false otherwise
     */
    template <typename TmxLayer>
    bool isTriggerLayer(TmxLayer && layer) {
        return std::any_of(layer.getProperties().begin(), layer.getProperties().end(), [](const auto &prop){
            return prop.getName().find("trigger") != std::string::npos;
        });
    }

} // end namespace map::algo

namespace fys::ws {

    // forward declaration
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
        TP_TRIGGER,
        NONE
    };

    class MapElement {

    public:
        constexpr void executePotentialTrigger(uint indexPlayer) const;

        /**
         * check if the element is of type BLOCK, if it is, check every collision blockers on the mapElement
         * to verify if the PlayerInfo collide with them.
         */
        inline bool canGoThrough(double x, double y, std::size_t level) const noexcept;

        void setLevel(std::size_t level) { _level.set(level); }
        void setChangeLevel(std::size_t level) { _changeLevel.set(level); }
        constexpr void setType(eElementType type) { _type = type; }
        void addCollision(const tmx::FloatRect &object) {
            _collisions.emplace_back(object);
        }

        template <typename T>
        constexpr void setTrigger(T && trigger) {
            _trigger = std::forward(trigger);
        }

    private:
        inline bool canGoToLevel(std::size_t goLevel) const noexcept;

    private:
        std::bitset<4> _level;
        std::bitset<4> _changeLevel; // set on stairs to pass from a level to another
        eElementType _type = eElementType::NONE;
        std::variant<ConnectionHandler *, void *> _trigger; // TODO: replace it with LUA script reference
        std::vector<tmx::FloatRect> _collisions;

    };

    class CollisionMap {

    public:
        CollisionMap(const WorldServerContext& ctx);

        CollisionMap(CollisionMap &&) noexcept = default;
        CollisionMap(const CollisionMap&) = delete;
        CollisionMap &operator=(const CollisionMap&) = delete;

        void buildMapFromTmx(const std::string &tmxMapPath);
        /**
         * Check if the position is in the boundary of the map before checking on the map
         * @return true if it is possible to move on the given position, false otherwise
         */
        bool canMoveTo(double x, double y, std::size_t level) const noexcept;
        void executePotentialTrigger(uint index, const PlayerInfo &positionOnMap, ws::ConnectionHandler &conn);

    private:
        std::pair<double, double> _boundaryX;
        std::pair<double, double> _boundaryY;
        std::vector<ProximityServer> _serverProximity;
        std::vector<std::vector<MapElement>> _mapElems;

    };

}

#endif //FYS_COLLISIONMAP_HH
