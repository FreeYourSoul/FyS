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

#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>
#include <array>

namespace fys::util {
class Key;
}

namespace fys::ws {

//! Default distance to check character around a point
constexpr static double DEFAULT_DISTANCE = 30;
//! Maximum number of character notified when a character move
constexpr static unsigned LIMIT_NOTIFICATIONS_MOVE = 50;

template<typename T>
struct Vector2 {
	T x = 0;
	T y = 0;

	Vector2 operator+(const Vector2& other) const
	{
		return {x + other.x, y + other.y};
	}

	Vector2 operator+(T value) const
	{
		return {x + value, y + value};
	}

	Vector2 operator-(const Vector2& other) const
	{
		return {x - other.x, y - other.y};
	}

	Vector2 operator-(T value) const
	{
		return {x - value, y - value};
	}

};

template<typename T>
struct Rectangle final {
	Rectangle()
			:left(0), top(0), width(0), height(0) { }
	Rectangle(T l, T t, T w, T h)
			:left(l), top(t), width(w), height(h) { }
	Rectangle(Vector2<T> position, Vector2<T> size)
			:left(position.x), top(position.y), width(size.x), height(size.y) { }
	T left, top, width, height;
};

using HitBoxd = Rectangle<double>;
using HitBoxu = Rectangle<uint>;
using Zone = Rectangle<uint>;
using Vec2u = Vector2<uint>;

//! Position in 2D space (X, Y)
using Pos = Vector2<double>;


struct PlayerInfo {
	Pos pos;
	double velocity = 1.0;
	double angle = 0.0;
};

enum class PlayerStatus {
	FIGHTING,
	MOVING,
	STANDING
};

class PlayersData {

	static constexpr uint MAXIMUM_PLAYER_CONNECTED_TO_WORLD_SERVER = 1000;

public:
	explicit PlayersData(uint maxConnection = MAXIMUM_PLAYER_CONNECTED_TO_WORLD_SERVER) noexcept;

	template<typename Action>
	void executeOnPlayers(Action&& actionToExecute)
	{
		for (uint i = 0; i < _status.size(); ++i) {
			std::forward<Action>(actionToExecute)(i, _status.at(i), _positions.at(i), _identities.at(i), _userNames.at(i));
		}
	}

	void setPlayerMoveAction(uint index, double direction)
	{
		_positions.at(index).angle = direction;
		_status.at(index) = PlayerStatus::MOVING;
	}

	void stopPlayerMove(uint index)
	{
		_status.at(index) = PlayerStatus::STANDING;
	}

	void setPlayerStatusInArena(uint index, const std::string& arenaId)
	{
		_status.at(index) = PlayerStatus::FIGHTING;
	}

	[[nodiscard]] uint
	addNewPlayerData(PlayerInfo info, std::string identity, std::string userName);

	/**
	 * @brief Get the Players Identities Around the given player except himself
	 *
	 * @param indexPlayer player
	 * @param position
	 * @param distance
	 * @return std::vector<std::string_view>
	 */
	[[nodiscard]] inline std::vector<std::string_view>
	getPlayerIdtsAroundPlayer(uint indexPlayer,
			std::optional<std::reference_wrapper<PlayerInfo>> position,
			double distance = DEFAULT_DISTANCE) const noexcept;

	/**
	 * @brief Get the Players Identities Around the given position
	 *
	 * @param position to check around
	 * @param distance radius around the given point to search
	 * @param ignoreIndex index that need to be ignored in the return
	 * @return std::vector<std::string_view> vector of view on the identities of the players around the given point
	 * @note identities are zmq code to reply to a specific client via a router socket
	 */
	[[nodiscard]] std::vector<std::string_view>
	getPlayerIdtsAroundPos(const Pos& position,
			double distance = DEFAULT_DISTANCE,
			uint ignoreIndex = LIMIT_NOTIFICATIONS_MOVE) const noexcept;

private:
	std::vector<PlayerInfo> _positions;
	std::vector<PlayerStatus> _status;
	std::vector<std::string> _identities;
	std::vector<std::string> _userNames;

};

}

#endif //FYS_PLAYERSDATA_HH
