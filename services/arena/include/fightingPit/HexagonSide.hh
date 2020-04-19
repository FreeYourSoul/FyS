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

#ifndef FYS_HEXAGONSIDE_HH
#define FYS_HEXAGONSIDE_HH

#include <utility>
#include <zconf.h>
#include <ostream>

namespace fys::arena {

class HexagonSide {
public:
	enum class Hexagon {
		A,
		B,
		C
	};

	static constexpr unsigned SIDE_NUMBER = 6 * 3; // sidesByHexagon * hexagonNumber

	enum class Orientation : int {
		A_N = 0,
		A_NE,
		A_SE,
		A_S,
		A_SW,
		A_NW,
		B_N,
		B_NE,
		B_SE,
		B_S,
		B_SW,
		B_NW,
		C_N,
		C_NE,
		C_SE,
		C_S,
		C_SW,
		C_NW,
		NONE // 18
	};

	explicit HexagonSide(Orientation orientation)
	{
		move(orientation, true);
	}

	explicit HexagonSide(Hexagon hexagon = Hexagon::A, Orientation orientation = Orientation::NONE)
			:
			_hexagon(hexagon), _side(orientation) { }

	[[nodiscard]] std::pair<Hexagon, Orientation>
	operator*() const { return std::pair(_hexagon, _side); }

	bool move(HexagonSide::Orientation destinationSide, bool forceMovement = false);
	bool moveRight();
	bool moveLeft();
	bool moveBack();

	[[nodiscard]] bool
	isAmbushSide() const;

	[[nodiscard]] bool
	canMove(HexagonSide::Orientation destinationSide) const;

private:
	void changeSide(HexagonSide::Orientation);

	[[nodiscard]] Orientation
	findLeft() const;

	[[nodiscard]] Orientation
	findRight() const;

	[[nodiscard]] Orientation
	findBack() const;

private:
	Hexagon _hexagon = Hexagon::A;
	Orientation _side = Orientation::NONE;
};

} // namespace fys::arena

std::ostream&
operator<<(std::ostream& os, fys::arena::HexagonSide::Orientation orientation);

std::ostream&
operator<<(std::ostream& os, fys::arena::HexagonSide::Hexagon hexagon);

std::ostream&
operator<<(std::ostream& os, fys::arena::HexagonSide hexagonSide);

#endif // !FYS_HEXAGONSIDE_HH
