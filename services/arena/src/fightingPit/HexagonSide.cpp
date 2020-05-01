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

#include <fightingPit/HexagonSide.hh>

namespace {
using h = fys::arena::HexagonSide::Orientation;

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
	isMoveable([[maybe_unused]]fys::arena::HexagonSide::Orientation dest) { return false; }
};

template<>
struct HexaSideOrient<HexaA, North> {
	constexpr static bool
	isMoveable(fys::arena::HexagonSide::Orientation dest) { return dest == h::A_NE || dest == h::A_NW; }
};

template<>
struct HexaSideOrient<HexaA, South> {
	constexpr static bool
	isMoveable(fys::arena::HexagonSide::Orientation dest)
	{
		return
				dest == h::A_SE || dest == h::A_SW ||
						dest == h::C_NW || dest == h::C_SW;
	}
};

template<>
struct HexaSideOrient<HexaA, NorthEast> {
	constexpr static bool
	isMoveable(fys::arena::HexagonSide::Orientation dest)
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
	isMoveable(fys::arena::HexagonSide::Orientation dest) { return dest == h::A_N || dest == h::A_SW; }
};

template<> // A_SE
struct HexaSideOrient<HexaA, SouthEast> {
	constexpr static bool
	isMoveable(fys::arena::HexagonSide::Orientation dest)
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
	isMoveable(fys::arena::HexagonSide::Orientation dest) { return dest == h::A_NW || dest == h::A_S; }
};

template<>
struct HexaSideOrient<HexaB, North> {
	constexpr static bool
	isMoveable(fys::arena::HexagonSide::Orientation dest) { return dest == h::B_NW || dest == h::B_NE; }
};

template<>
struct HexaSideOrient<HexaB, NorthEast> {
	constexpr static bool
	isMoveable(fys::arena::HexagonSide::Orientation dest) { return dest == h::B_N || dest == h::B_SE; }
};

template<>
struct HexaSideOrient<HexaB, NorthWest> {
	constexpr static bool
	isMoveable(fys::arena::HexagonSide::Orientation dest)
	{
		return dest == h::B_N || dest == h::B_SW ||
				dest == h::A_N || dest == h::A_NE;
	}
};

template<>
struct HexaSideOrient<HexaB, South> {
	constexpr static bool
	isMoveable(fys::arena::HexagonSide::Orientation dest)
	{
		return dest == h::B_SW || dest == h::B_SE ||
				dest == h::C_N || dest == h::C_NE || dest == h::C_NW ||
				dest == h::A_NE || dest == h::A_SE;
	}
};

template<>
struct HexaSideOrient<HexaB, SouthEast> {
	constexpr static bool
	isMoveable(fys::arena::HexagonSide::Orientation dest)
	{
		return dest == h::B_S || dest == h::B_NE ||
				dest == h::C_N || dest == h::C_NE;
	}
};

template<>
struct HexaSideOrient<HexaB, SouthWest> {
	constexpr static bool
	isMoveable(fys::arena::HexagonSide::Orientation dest)
	{
		return dest == h::B_NW || dest == h::B_S ||
				dest == h::C_N || dest == h::C_NW ||
				dest == h::A_NE || dest == h::A_SE || dest == h::A_N;
	}
};

template<>
struct HexaSideOrient<HexaC, North> {
	constexpr static bool
	isMoveable(fys::arena::HexagonSide::Orientation dest)
	{
		return dest == h::C_NE || dest == h::C_NW ||
				dest == h::B_S || dest == h::B_SE || dest == h::B_SW ||
				dest == h::A_NE || dest == h::A_SE;
	}
};

template<>
struct HexaSideOrient<HexaC, NorthEast> {
	constexpr static bool
	isMoveable(fys::arena::HexagonSide::Orientation dest)
	{
		return dest == h::C_N || dest == h::C_SE ||
				dest == h::B_S || dest == h::B_SE;
	}
};

template<>
struct HexaSideOrient<HexaC, NorthWest> {
	constexpr static bool
	isMoveable(fys::arena::HexagonSide::Orientation dest)
	{
		return dest == h::C_N || dest == h::C_SW ||
				dest == h::A_SE || dest == h::A_S || dest == h::A_NE ||
				dest == h::B_S || dest == h::B_SW;
	}
};

template<>
struct HexaSideOrient<HexaC, South> {
	constexpr static bool
	isMoveable(fys::arena::HexagonSide::Orientation dest) { return dest == h::C_SW || dest == h::C_SE; }
};

template<>
struct HexaSideOrient<HexaC, SouthWest> {
	constexpr static bool
	isMoveable(fys::arena::HexagonSide::Orientation dest)
	{
		return dest == h::C_S || dest == h::C_NW ||
				dest == h::A_S || dest == h::A_SE;
	}
};

template<>
struct HexaSideOrient<HexaC, SouthEast> {
	constexpr static bool
	isMoveable(fys::arena::HexagonSide::Orientation dest) { return dest == h::C_S || dest == h::C_NE; }
};

// Specialization to check the hexagon
template<typename Hexa>
struct HexaSide;

template<>
struct HexaSide<HexaA> {
	constexpr static bool
	isMoveable(fys::arena::HexagonSide::Orientation origin, fys::arena::HexagonSide::Orientation dest)
	{
		if (origin == dest)
			return false;
		if (origin == fys::arena::HexagonSide::Orientation::A_S)
			return HexaSideOrient<HexaA, South>::isMoveable(dest);
		else if (origin == fys::arena::HexagonSide::Orientation::A_SE)
			return HexaSideOrient<HexaA, SouthEast>::isMoveable(dest);
		else if (origin == fys::arena::HexagonSide::Orientation::A_SW)
			return HexaSideOrient<HexaA, SouthWest>::isMoveable(dest);
		else if (origin == fys::arena::HexagonSide::Orientation::A_N)
			return HexaSideOrient<HexaA, North>::isMoveable(dest);
		else if (origin == fys::arena::HexagonSide::Orientation::A_NE)
			return HexaSideOrient<HexaA, NorthEast>::isMoveable(dest);
		else if (origin == fys::arena::HexagonSide::Orientation::A_NW)
			return HexaSideOrient<HexaA, NorthWest>::isMoveable(dest);
		return false;
	}
};

template<>
struct HexaSide<HexaB> {
	constexpr static bool
	isMoveable(fys::arena::HexagonSide::Orientation origin, fys::arena::HexagonSide::Orientation dest)
	{
		if (origin == dest)
			return false;
		if (origin == fys::arena::HexagonSide::Orientation::B_S)
			return HexaSideOrient<HexaB, South>::isMoveable(dest);
		else if (origin == fys::arena::HexagonSide::Orientation::B_SE)
			return HexaSideOrient<HexaB, SouthEast>::isMoveable(dest);
		else if (origin == fys::arena::HexagonSide::Orientation::B_SW)
			return HexaSideOrient<HexaB, SouthWest>::isMoveable(dest);
		if (origin == fys::arena::HexagonSide::Orientation::B_N)
			return HexaSideOrient<HexaB, North>::isMoveable(dest);
		else if (origin == fys::arena::HexagonSide::Orientation::B_NE)
			return HexaSideOrient<HexaB, NorthEast>::isMoveable(dest);
		else if (origin == fys::arena::HexagonSide::Orientation::B_NW)
			return HexaSideOrient<HexaB, NorthWest>::isMoveable(dest);
		return false;
	}
};

template<>
struct HexaSide<HexaC> {
	constexpr static bool
	isMoveable(fys::arena::HexagonSide::Orientation origin, fys::arena::HexagonSide::Orientation dest)
	{
		if (origin == dest)
			return false;
		if (origin == fys::arena::HexagonSide::Orientation::C_S)
			return HexaSideOrient<HexaC, South>::isMoveable(dest);
		else if (origin == fys::arena::HexagonSide::Orientation::C_SE)
			return HexaSideOrient<HexaC, SouthEast>::isMoveable(dest);
		else if (origin == fys::arena::HexagonSide::Orientation::C_SW)
			return HexaSideOrient<HexaC, SouthWest>::isMoveable(dest);
		if (origin == fys::arena::HexagonSide::Orientation::C_N)
			return HexaSideOrient<HexaC, North>::isMoveable(dest);
		else if (origin == fys::arena::HexagonSide::Orientation::C_NE)
			return HexaSideOrient<HexaC, NorthEast>::isMoveable(dest);
		else if (origin == fys::arena::HexagonSide::Orientation::C_NW)
			return HexaSideOrient<HexaC, NorthWest>::isMoveable(dest);
		return false;
	}
};

} // namespace anonyme


namespace fys::arena {

bool
HexagonSide::move(HexagonSide::Orientation destinationSide, bool forceMovement)
{
	if (forceMovement || (destinationSide != HexagonSide::Orientation::NONE && canMove(destinationSide))) {
		return changeSide(destinationSide);
	}
	return false;
}

bool
HexagonSide::moveLeft()
{
	return move(findLeft());
}

bool
HexagonSide::moveRight()
{
	return move(findRight());
}

bool
HexagonSide::moveBack()
{
	return move(findBack());
}

// private API

bool
HexagonSide::changeSide(HexagonSide::Orientation destination)
{
	switch (destination) {
		case HexagonSide::Orientation::A_N:
		case HexagonSide::Orientation::A_NE:
		case HexagonSide::Orientation::A_NW:
		case HexagonSide::Orientation::A_S:
		case HexagonSide::Orientation::A_SE:
		case HexagonSide::Orientation::A_SW:
			_hexagon = HexagonSide::Hexagon::A;
			_side = destination;
			return true;
		case HexagonSide::Orientation::B_N:
		case HexagonSide::Orientation::B_NE:
		case HexagonSide::Orientation::B_NW:
		case HexagonSide::Orientation::B_S:
		case HexagonSide::Orientation::B_SE:
		case HexagonSide::Orientation::B_SW:
			_hexagon = HexagonSide::Hexagon::B;
			_side = destination;
			return true;
		case HexagonSide::Orientation::C_N:
		case HexagonSide::Orientation::C_NE:
		case HexagonSide::Orientation::C_NW:
		case HexagonSide::Orientation::C_S:
		case HexagonSide::Orientation::C_SE:
		case HexagonSide::Orientation::C_SW:
			_hexagon = HexagonSide::Hexagon::C;
			_side = destination;
			return true;
		default: return false;
	}
	return false;
}

HexagonSide::Orientation
HexagonSide::findLeft() const
{
	if (_side == HexagonSide::Orientation::A_N)
		return HexagonSide::Orientation::A_NW;
	else if (_side == HexagonSide::Orientation::B_N)
		return HexagonSide::Orientation::B_NW;
	else if (_side == HexagonSide::Orientation::C_N)
		return HexagonSide::Orientation::C_NW;
	return static_cast<HexagonSide::Orientation >(static_cast<int>(_side) - 1);
}

HexagonSide::Orientation
HexagonSide::findRight() const
{
	if (_side == HexagonSide::Orientation::A_NW)
		return HexagonSide::Orientation::A_N;
	else if (_side == HexagonSide::Orientation::B_NW)
		return HexagonSide::Orientation::B_N;
	else if (_side == HexagonSide::Orientation::C_NW)
		return HexagonSide::Orientation::C_N;
	return static_cast<HexagonSide::Orientation >(static_cast<int>(_side) + 1);
}

HexagonSide::Orientation
HexagonSide::findBack() const
{
	if (!isAmbushSide())
		return HexagonSide::Orientation::NONE;

	else if (_side == HexagonSide::Orientation::B_S)
		return HexagonSide::Orientation::C_N;
	else if (_side == HexagonSide::Orientation::C_N)
		return HexagonSide::Orientation::B_S;

	else if (_side == HexagonSide::Orientation::A_NE)
		return HexagonSide::Orientation::B_SW;
	else if (_side == HexagonSide::Orientation::B_SW)
		return HexagonSide::Orientation::A_NE;

	else if (_side == HexagonSide::Orientation::C_NW)
		return HexagonSide::Orientation::A_SE;
	else if (_side == HexagonSide::Orientation::A_SE)
		return HexagonSide::Orientation::C_NW;
	return HexagonSide::Orientation::NONE;
}

bool
HexagonSide::isAmbushSide() const
{
	return _side == Orientation::A_NE || _side == Orientation::A_SE ||
			_side == Orientation::B_S || _side == Orientation::B_SW ||
			_side == Orientation::C_N || _side == Orientation::C_NW;
}

bool
HexagonSide::canMove(HexagonSide::Orientation destinationSide) const
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
operator<<(std::ostream& os, fys::arena::HexagonSide::Orientation orientation)
{
	switch (orientation) {
		case fys::arena::HexagonSide::Orientation::A_N:
		case fys::arena::HexagonSide::Orientation::B_N:
		case fys::arena::HexagonSide::Orientation::C_N:return os << "[North]";
		case fys::arena::HexagonSide::Orientation::A_S:
		case fys::arena::HexagonSide::Orientation::B_S:
		case fys::arena::HexagonSide::Orientation::C_S:return os << "[South]";
		case fys::arena::HexagonSide::Orientation::A_NE:
		case fys::arena::HexagonSide::Orientation::B_NE:
		case fys::arena::HexagonSide::Orientation::C_NE:return os << "[NorthEast]";
		case fys::arena::HexagonSide::Orientation::A_NW:
		case fys::arena::HexagonSide::Orientation::B_NW:
		case fys::arena::HexagonSide::Orientation::C_NW:return os << "[NorthWest]";
		case fys::arena::HexagonSide::Orientation::A_SE:
		case fys::arena::HexagonSide::Orientation::B_SE:
		case fys::arena::HexagonSide::Orientation::C_SE:return os << "[SouthEast]";
		case fys::arena::HexagonSide::Orientation::A_SW:
		case fys::arena::HexagonSide::Orientation::B_SW:
		case fys::arena::HexagonSide::Orientation::C_SW:return os << "[SouthWest]";
		case fys::arena::HexagonSide::Orientation::NONE:return os << "[NONE(ERROR)]";
	}
	return os;
}

std::ostream&
operator<<(std::ostream& os, fys::arena::HexagonSide::Hexagon hexagon)
{
	switch (hexagon) {
		case fys::arena::HexagonSide::Hexagon::A:return os << "[A]";
		case fys::arena::HexagonSide::Hexagon::B:return os << "[B]";
		case fys::arena::HexagonSide::Hexagon::C:return os << "[C]";
	}
	return os;
}

std::ostream&
operator<<(std::ostream& os, fys::arena::HexagonSide hexagonSide)
{
	auto hs = *hexagonSide;
	return os << "HexagonSide:" << hs.first << hs.second;
}
