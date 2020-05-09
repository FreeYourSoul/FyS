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

namespace fys::ws {

namespace {

[[nodiscard]]unsigned
getX(double x, unsigned tileSizeX)
{
	static const unsigned gTileSizeX = tileSizeX;
	return static_cast<unsigned>(x) / gTileSizeX;
}

[[nodiscard]] unsigned
getY(double y, unsigned tileSizeY)
{
	static const unsigned gTileSizeY = tileSizeY;
	return static_cast<unsigned>(y) / gTileSizeY;
}
}

// CollisionMap
CollisionMap::CollisionMap(const fys::ws::WorldServerContext& ctx)
		:
		_boundaryX(ctx.getServerXBoundaries()),
		_boundaryY(ctx.getServerYBoundaries()),
		_serverProximity(ctx.getServerProximity())
{
	buildMapFromTmx(ctx.getTMXMapPath());
}

void
CollisionMap::buildMapFromTmx(const std::string& tmxMapPath)
{
	tmx::Map map;
	if (!map.load(tmxMapPath)) {
		SPDLOG_ERROR("TMX CollisionMap couldn't be loaded");
		return;
	}
	const auto& layers = map.getLayers();

	_mapElems.resize(map.getTileCount().y);
	for (auto& elemOnY : _mapElems)
		elemOnY.resize(map.getTileCount().x);

	for (const auto& layer : layers) {
		if (layer->getType() == tmx::Layer::Type::Object) {
			const auto& objectLayer = layer->getLayerAs<tmx::ObjectGroup>();

			if (map::algo::isCollisionLayer(objectLayer)) {
				addCollisionInMap({map.getTileSize().x, map.getTileSize().y}, objectLayer);
			}
			else if (map::algo::isTriggerLayer(objectLayer)) {
				addTriggerInMap(objectLayer);
			}
		}
	}
}

void
CollisionMap::addCollisionInMap(const Vec2u& tileMapSize, const tmx::ObjectGroup& collisionLayer)
{
	const auto& objects = collisionLayer.getObjects();
	for (const auto& object : objects) {
		for (auto y = getX(object.getAABB().top, tileMapSize.y);
			 y < getX(object.getAABB().top + object.getAABB().height, tileMapSize.y); ++y) {
			for (auto x = getY(object.getAABB().left, tileMapSize.x);
				 x < getY(object.getAABB().left + object.getAABB().width, tileMapSize.x); ++x) {
				_mapElems[y][x].setType(eElementType::BLOCK);
				_mapElems[y][x].addCollision(object.getAABB());
			}
		}
	}
}

void
CollisionMap::addTriggerInMap(const tmx::ObjectGroup& triggerLayer)
{
	// TODO : Add triggers on the map
}

void
CollisionMap::executePotentialTrigger(uint index, const PlayerInfo& positionOnMap)
{
}

bool
CollisionMap::canMoveTo(Pos pos, std::size_t level) const noexcept
{
	if (pos.x < _boundaryX.in || pos.x > _boundaryX.out || pos.y < _boundaryY.in || pos.y > _boundaryY.out)
		return false;
	return _mapElems[static_cast<unsigned long>(pos.x)][static_cast<unsigned long>(pos.y)].canGoThrough(pos, level);
}

// CollisionMap Element
void
MapElement::executePotentialTrigger(uint indexPlayer) const
{
	if (_type == eElementType::TRIGGER) {

	}
	else if (_type == eElementType::TP_TRIGGER) {

	}
}

bool
MapElement::canGoThrough(Pos position, std::size_t level) const noexcept
{
	bool canGoThrough = canGoToLevel(level);
	if (canGoThrough && _type == eElementType::BLOCK) {
		return std::none_of(_collisions.begin(), _collisions.end(), [&position](const auto& aabb) {
			return (position.x >= aabb.left && position.x <= aabb.left + aabb.width &&
					position.y >= aabb.top && position.y <= aabb.top + aabb.height);
		});
	}
	return canGoThrough;
}

bool
MapElement::canGoToLevel(std::size_t goLevel) const noexcept
{
	return goLevel == 0 || _level.test(goLevel) || _changeLevel.test(goLevel);
}

}
