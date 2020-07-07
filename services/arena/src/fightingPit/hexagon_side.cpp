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

#include <fightingPit/hexagon_side.hh>

namespace {
using h = fys::arena::hexagon_side::orientation;

struct HexaA;
struct HexaB;
struct HexaC;
struct South;
struct SouthEast;
struct SouthWest;
struct North;
struct NorthEast;
struct NorthWest;

// Specialization to check the orientation
template<typename ID, typename Orient>
struct HexaSideOrient {
	constexpr static bool
	is_movable([[maybe_unused]]fys::arena::hexagon_side::orientation dest) { return false; }
};

template<>
struct HexaSideOrient<HexaA, North> {
	constexpr static bool
	is_movable(fys::arena::hexagon_side::orientation dest) { return dest == h::A_NE || dest == h::A_NW; }
};

template<>
struct HexaSideOrient<HexaA, South> {
	constexpr static bool
	is_movable(fys::arena::hexagon_side::orientation dest)
	{
		return
				dest == h::A_SE || dest == h::A_SW ||
						dest == h::C_NW || dest == h::C_SW;
	}
};

template<>
struct HexaSideOrient<HexaA, NorthEast> {
	constexpr static bool
	is_movable(fys::arena::hexagon_side::orientation dest)
	{
		return
				dest == h::A_N || dest == h::A_SE ||
						dest == h::B_S || dest == h::B_NW || dest == h::B_SW ||
						dest == h::C_N || dest == h::C_NW;
	}
};

template<>
struct HexaSideOrient<HexaA, NorthWest> {
	constexpr static bool
	is_movable(fys::arena::hexagon_side::orientation dest) { return dest == h::A_N || dest == h::A_SW; }
};

template<> // A_SE
struct HexaSideOrient<HexaA, SouthEast> {
	constexpr static bool
	is_movable(fys::arena::hexagon_side::orientation dest)
	{
		return
				dest == h::A_NE || dest == h::A_S ||
						dest == h::B_S || dest == h::B_SW ||
						dest == h::C_N || dest == h::C_NW || dest == h::C_SW;
	}
};

template<> // A_SW
struct HexaSideOrient<HexaA, SouthWest> {
	constexpr static bool
	is_movable(fys::arena::hexagon_side::orientation dest) { return dest == h::A_NW || dest == h::A_S; }
};

template<>
struct HexaSideOrient<HexaB, North> {
	constexpr static bool
	is_movable(fys::arena::hexagon_side::orientation dest) { return dest == h::B_NW || dest == h::B_NE; }
};

template<>
struct HexaSideOrient<HexaB, NorthEast> {
	constexpr static bool
	is_movable(fys::arena::hexagon_side::orientation dest) { return dest == h::B_N || dest == h::B_SE; }
};

template<>
struct HexaSideOrient<HexaB, NorthWest> {
	constexpr static bool
	is_movable(fys::arena::hexagon_side::orientation dest)
	{
		return dest == h::B_N || dest == h::B_SW ||
				dest == h::A_N || dest == h::A_NE;
	}
};

template<>
struct HexaSideOrient<HexaB, South> {
	constexpr static bool
	is_movable(fys::arena::hexagon_side::orientation dest)
	{
		return dest == h::B_SW || dest == h::B_SE ||
				dest == h::C_N || dest == h::C_NE || dest == h::C_NW ||
				dest == h::A_NE || dest == h::A_SE;
	}
};

template<>
struct HexaSideOrient<HexaB, SouthEast> {
	constexpr static bool
	is_movable(fys::arena::hexagon_side::orientation dest)
	{
		return dest == h::B_S || dest == h::B_NE ||
				dest == h::C_N || dest == h::C_NE;
	}
};

template<>
struct HexaSideOrient<HexaB, SouthWest> {
	constexpr static bool
	is_movable(fys::arena::hexagon_side::orientation dest)
	{
		return dest == h::B_NW || dest == h::B_S ||
				dest == h::C_N || dest == h::C_NW ||
				dest == h::A_NE || dest == h::A_SE || dest == h::A_N;
	}
};

template<>
struct HexaSideOrient<HexaC, North> {
	constexpr static bool
	is_movable(fys::arena::hexagon_side::orientation dest)
	{
		return dest == h::C_NE || dest == h::C_NW ||
				dest == h::B_S || dest == h::B_SE || dest == h::B_SW ||
				dest == h::A_NE || dest == h::A_SE;
	}
};

template<>
struct HexaSideOrient<HexaC, NorthEast> {
	constexpr static bool
	is_movable(fys::arena::hexagon_side::orientation dest)
	{
		return dest == h::C_N || dest == h::C_SE ||
				dest == h::B_S || dest == h::B_SE;
	}
};

template<>
struct HexaSideOrient<HexaC, NorthWest> {
	constexpr static bool
	is_movable(fys::arena::hexagon_side::orientation dest)
	{
		return dest == h::C_N || dest == h::C_SW ||
				dest == h::A_SE || dest == h::A_S || dest == h::A_NE ||
				dest == h::B_S || dest == h::B_SW;
	}
};

template<>
struct HexaSideOrient<HexaC, South> {
	constexpr static bool
	is_movable(fys::arena::hexagon_side::orientation dest) { return dest == h::C_SW || dest == h::C_SE; }
};

template<>
struct HexaSideOrient<HexaC, SouthWest> {
	constexpr static bool
	is_movable(fys::arena::hexagon_side::orientation dest)
	{
		return dest == h::C_S || dest == h::C_NW ||
				dest == h::A_S || dest == h::A_SE;
	}
};

template<>
struct HexaSideOrient<HexaC, SouthEast> {
	constexpr static bool
	is_movable(fys::arena::hexagon_side::orientation dest) { return dest == h::C_S || dest == h::C_NE; }
};

// Specialization to check the hexagon
template<typename Hexa>
struct HexaSide;

template<>
struct HexaSide<HexaA> {
	constexpr static bool
	is_movable(fys::arena::hexagon_side::orientation origin, fys::arena::hexagon_side::orientation dest)
	{
		if (origin == dest)
			return false;
		if (origin == fys::arena::hexagon_side::orientation::A_S)
			return HexaSideOrient<HexaA, South>::isMoveable(dest);
		else if (origin == fys::arena::hexagon_side::orientation::A_SE)
			return HexaSideOrient<HexaA, SouthEast>::isMoveable(dest);
		else if (origin == fys::arena::hexagon_side::orientation::A_SW)
			return HexaSideOrient<HexaA, SouthWest>::isMoveable(dest);
		else if (origin == fys::arena::hexagon_side::orientation::A_N)
			return HexaSideOrient<HexaA, North>::isMoveable(dest);
		else if (origin == fys::arena::hexagon_side::orientation::A_NE)
			return HexaSideOrient<HexaA, NorthEast>::isMoveable(dest);
		else if (origin == fys::arena::hexagon_side::orientation::A_NW)
			return HexaSideOrient<HexaA, NorthWest>::isMoveable(dest);
		return false;
	}
};

template<>
struct HexaSide<HexaB> {
	constexpr static bool
	is_movable(fys::arena::hexagon_side::orientation origin, fys::arena::hexagon_side::orientation dest)
	{
		if (origin == dest)
			return false;
		if (origin == fys::arena::hexagon_side::orientation::B_S)
			return HexaSideOrient<HexaB, South>::isMoveable(dest);
		else if (origin == fys::arena::hexagon_side::orientation::B_SE)
			return HexaSideOrient<HexaB, SouthEast>::isMoveable(dest);
		else if (origin == fys::arena::hexagon_side::orientation::B_SW)
			return HexaSideOrient<HexaB, SouthWest>::isMoveable(dest);
		if (origin == fys::arena::hexagon_side::orientation::B_N)
			return HexaSideOrient<HexaB, North>::isMoveable(dest);
		else if (origin == fys::arena::hexagon_side::orientation::B_NE)
			return HexaSideOrient<HexaB, NorthEast>::isMoveable(dest);
		else if (origin == fys::arena::hexagon_side::orientation::B_NW)
			return HexaSideOrient<HexaB, NorthWest>::isMoveable(dest);
		return false;
	}
};

template<>
struct HexaSide<HexaC> {
	constexpr static bool
	is_movable(fys::arena::hexagon_side::orientation origin, fys::arena::hexagon_side::orientation dest)
	{
		if (origin == dest)
			return false;
		if (origin == fys::arena::hexagon_side::orientation::C_S)
			return HexaSideOrient<HexaC, South>::isMoveable(dest);
		else if (origin == fys::arena::hexagon_side::orientation::C_SE)
			return HexaSideOrient<HexaC, SouthEast>::isMoveable(dest);
		else if (origin == fys::arena::hexagon_side::orientation::C_SW)
			return HexaSideOrient<HexaC, SouthWest>::isMoveable(dest);
		if (origin == fys::arena::hexagon_side::orientation::C_N)
			return HexaSideOrient<HexaC, North>::isMoveable(dest);
		else if (origin == fys::arena::hexagon_side::orientation::C_NE)
			return HexaSideOrient<HexaC, NorthEast>::isMoveable(dest);
		else if (origin == fys::arena::hexagon_side::orientation::C_NW)
			return HexaSideOrient<HexaC, NorthWest>::isMoveable(dest);
		return false;
	}
};

} // namespace anonyme


namespace fys::arena {

bool
hexagon_side::move(hexagon_side::orientation destination_side, bool force_movement)
{
	if (force_movement || (destination_side != hexagon_side::orientation::NONE && canMove(destination_side))) {
		return changeSide(destination_side);
	}
	return false;
}

bool
hexagon_side::changeSide(hexagon_side::orientation destination)
{
	switch (destination) {
		case hexagon_side::orientation::A_N:
		case hexagon_side::orientation::A_NE:
		case hexagon_side::orientation::A_NW:
		case hexagon_side::orientation::A_S:
		case hexagon_side::orientation::A_SE:
		case hexagon_side::orientation::A_SW:
			_hexagon = hexagon_side::Hexagon::A;
			_side = destination;
			return true;
		case hexagon_side::orientation::B_N:
		case hexagon_side::orientation::B_NE:
		case hexagon_side::orientation::B_NW:
		case hexagon_side::orientation::B_S:
		case hexagon_side::orientation::B_SE:
		case hexagon_side::orientation::B_SW:
			_hexagon = hexagon_side::Hexagon::B;
			_side = destination;
			return true;
		case hexagon_side::orientation::C_N:
		case hexagon_side::orientation::C_NE:
		case hexagon_side::orientation::C_NW:
		case hexagon_side::orientation::C_S:
		case hexagon_side::orientation::C_SE:
		case hexagon_side::orientation::C_SW:
			_hexagon = hexagon_side::Hexagon::C;
			_side = destination;
			return true;
		default: return false;
	}
	return false;
}

hexagon_side::orientation
hexagon_side::findLeft() const
{
	if (_side == hexagon_side::orientation::A_N)
		return hexagon_side::orientation::A_NW;
	else if (_side == hexagon_side::orientation::B_N)
		return hexagon_side::orientation::B_NW;
	else if (_side == hexagon_side::orientation::C_N)
		return hexagon_side::orientation::C_NW;
	return static_cast<hexagon_side::orientation >(static_cast<int>(_side) - 1);
}

hexagon_side::orientation
hexagon_side::findRight() const
{
	if (_side == hexagon_side::orientation::A_NW)
		return hexagon_side::orientation::A_N;
	else if (_side == hexagon_side::orientation::B_NW)
		return hexagon_side::orientation::B_N;
	else if (_side == hexagon_side::orientation::C_NW)
		return hexagon_side::orientation::C_N;
	return static_cast<hexagon_side::orientation >(static_cast<int>(_side) + 1);
}

hexagon_side::orientation
hexagon_side::findBack() const
{
	if (!isAmbushSide())
		return hexagon_side::orientation::NONE;

	else if (_side == hexagon_side::orientation::B_S)
		return hexagon_side::orientation::C_N;
	else if (_side == hexagon_side::orientation::C_N)
		return hexagon_side::orientation::B_S;

	else if (_side == hexagon_side::orientation::A_NE)
		return hexagon_side::orientation::B_SW;
	else if (_side == hexagon_side::orientation::B_SW)
		return hexagon_side::orientation::A_NE;

	else if (_side == hexagon_side::orientation::C_NW)
		return hexagon_side::orientation::A_SE;
	else if (_side == hexagon_side::orientation::A_SE)
		return hexagon_side::orientation::C_NW;
	return hexagon_side::orientation::NONE;
}

bool
hexagon_side::isAmbushSide() const
{
	return _side == orientation::A_NE || _side == orientation::A_SE ||
			_side == orientation::B_S || _side == orientation::B_SW ||
			_side == orientation::C_N || _side == orientation::C_NW;
}

bool
hexagon_side::canMove(hexagon_side::orientation destinationSide) const
{
	if (_hexagon == Hexagon::A)
		return HexaSide<HexaA>::isMoveable(_side, destinationSide);
	if (_hexagon == Hexagon::B)
		return HexaSide<HexaB>::isMoveable(_side, destinationSide);
	if (_hexagon == Hexagon::C)
		return HexaSide<HexaC>::isMoveable(_side, destinationSide);
	return false;
}

} // namespace fys::arena

std::ostream&
operator<<(std::ostream& os, fys::arena::hexagon_side::orientation orientation)
{
	switch (orientation) {
		case fys::arena::hexagon_side::orientation::A_N:
		case fys::arena::hexagon_side::orientation::B_N:
		case fys::arena::hexagon_side::orientation::C_N:return os << "[North]";
		case fys::arena::hexagon_side::orientation::A_S:
		case fys::arena::hexagon_side::orientation::B_S:
		case fys::arena::hexagon_side::orientation::C_S:return os << "[South]";
		case fys::arena::hexagon_side::orientation::A_NE:
		case fys::arena::hexagon_side::orientation::B_NE:
		case fys::arena::hexagon_side::orientation::C_NE:return os << "[NorthEast]";
		case fys::arena::hexagon_side::orientation::A_NW:
		case fys::arena::hexagon_side::orientation::B_NW:
		case fys::arena::hexagon_side::orientation::C_NW:return os << "[NorthWest]";
		case fys::arena::hexagon_side::orientation::A_SE:
		case fys::arena::hexagon_side::orientation::B_SE:
		case fys::arena::hexagon_side::orientation::C_SE:return os << "[SouthEast]";
		case fys::arena::hexagon_side::orientation::A_SW:
		case fys::arena::hexagon_side::orientation::B_SW:
		case fys::arena::hexagon_side::orientation::C_SW:return os << "[SouthWest]";
		case fys::arena::hexagon_side::orientation::NONE:return os << "[NONE(ERROR)]";
	}
	return os;
}

std::ostream&
operator<<(std::ostream& os, fys::arena::hexagon_side::Hexagon hexagon)
{
	switch (hexagon) {
		case fys::arena::hexagon_side::Hexagon::A:return os << "[A]";
		case fys::arena::hexagon_side::Hexagon::B:return os << "[B]";
		case fys::arena::hexagon_side::Hexagon::C:return os << "[C]";
	}
	return os;
}

std::ostream&
operator<<(std::ostream& os, fys::arena::hexagon_side hexagonSide)
{
	auto hs = *hexagonSide;
	return os << "HexagonSide:" << hs.first << hs.second;
}
