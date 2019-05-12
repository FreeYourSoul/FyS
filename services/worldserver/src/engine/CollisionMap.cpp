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

#include <spdlog/spdlog.h>
#include <tmxlite/Map.hpp>
#include <tmxlite/Layer.hpp>
#include <tmxlite/TileLayer.hpp>
#include <WorldServerContext.hh>
#include <iostream>
#include "engine/CollisionMap.hh"

// Variant visitor trick
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace fys::ws {

    // CollisionMap
    CollisionMap::CollisionMap(const fys::ws::WorldServerContext &ctx) :
        _boundaryX(ctx.getServerXBoundaries()),
        _boundaryY(ctx.getServerYBoundaries()),
        _serverProximity(ctx.getServerProximity()) {
        buildMapFromTmx(ctx.getTMXMapPath());
    }

    void CollisionMap::buildMapFromTmx(const std::string &tmxMapPath) {
        tmx::Map map;
        if (map.load(tmxMapPath)) {
            const auto& layers = map.getLayers();

            _mapElems.resize(map.getTileCount().y);
            for (auto &elemOnY : _mapElems)
                elemOnY.resize(map.getTileCount().x);

            for (const auto& layer : layers) {
                if (layer->getType() == tmx::Layer::Type::Object) {
                    const auto& objectLayer = layer->getLayerAs<tmx::ObjectGroup>();

                    if (map::algo::isCollisionLayer(objectLayer)) {
                        const auto& objects = objectLayer.getObjects();
                        for (const auto& object : objects) {
                            for (auto y = getX(object.getAABB().top, map.getTileSize().y);
                                y < getX(object.getAABB().top + object.getAABB().height, map.getTileSize().y); ++y)
                            {
                                for (auto x = getY(object.getAABB().left, map.getTileSize().x);
                                    x < getY(object.getAABB().left + object.getAABB().width, map.getTileSize().x); ++x)
                                {
                                    _mapElems[y][x].setType(eElementType::BLOCK);
                                    _mapElems[y][x].addCollision(object.getAABB());
                                }
                            }
                        }
                    }
                }
                else if (layer->getType() == tmx::Layer::Type::Tile) {
                    if (const auto& tileLayer = layer->getLayerAs<tmx::TileLayer>();
                        map::algo::isNotCosmeticLayer(tileLayer)) {

                    }
                }
            }
        } else {
            SPDLOG_ERROR("TMX CollisionMap couldn't be loaded");
        }
    }

    bool CollisionMap::canMoveTo(double x, double y, std::size_t level) const noexcept {
        if (x < _boundaryX.first || x > _boundaryX.second || y < _boundaryY.first || y > _boundaryY.second)
            return false;
        return _mapElems[static_cast<unsigned long>(x)][static_cast<unsigned long>(y)].canGoThrough(x, y, level);
    }

    // CollisionMap Element
    bool MapElement::canGoThrough(double x, double y, std::size_t level) const noexcept {
//        bool canGoThrough = canGoToLevel(level);
//        if (canGoThrough && _type == eElementType::BLOCK) {
//            for (const auto &col : _collisions) {
//              const auto &aabb = col.getAABB();
//              if (x >= aabb.left && x <= aabb.left + aabb.width &&
//                  y >= aabb.top && y <= aabb.top + aabb.height)
//                 return false;
//            }
//        }
//        return canGoThrough;
        return canGoToLevel(level) && _type != eElementType::BLOCK;
    }

    bool MapElement::canGoToLevel(std::size_t goLevel) const noexcept {
        return _level.test(goLevel) || _changeLevel.test(goLevel);
    }

    constexpr void MapElement::executePotentialTrigger(const std::string &token) const {
        if (_type == eElementType::TRIGGER) {
    //        std::visit(overloaded {
    //            [](ConnectionHandler &trigger, const std::string &token) { // Player Trigger
    //
    //            },
    //            [](void *trigger, const std::string &id) { // NPC Trigger
    //
    //            }
    //        }, _trigger, token);
        }
    }

    unsigned getX(double x, unsigned tileSizeX)  {
        static const unsigned gTileSizeX = tileSizeX;
        return static_cast<unsigned>(x) / gTileSizeX;
    }

    unsigned getY(double y, unsigned tileSizeY)  {
        static const unsigned gTileSizeY = tileSizeY;
        return static_cast<unsigned>(y) / gTileSizeY;
    }

}