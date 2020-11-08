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

#include <ostream>
#include <utility>
#include <zconf.h>

namespace fys::arena {

class hexagon_side {
public:
  enum class hexagon {
	A,
	B,
	C
  };

  static constexpr unsigned SIDE_NUMBER = 6 * 3;// sidesByHexagon * hexagonNumber

  enum class orientation : int {
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
	NONE// 18
  };

  explicit hexagon_side(orientation orientation) {
	move(orientation, true);
  }

  explicit hexagon_side(hexagon hexagon = hexagon::A, orientation orientation = orientation::NONE)
	  : _hexagon(hexagon), _side(orientation) {}

  [[nodiscard]] std::pair<hexagon, orientation>
  operator*() const { return std::pair(_hexagon, _side); }

  bool move(hexagon_side::orientation destination_side, bool force_movement = false);

  [[nodiscard]] bool
  is_ambush_side() const;

  [[nodiscard]] bool
  can_move_to(hexagon_side::orientation destination_side) const;

  [[nodiscard]] orientation
  find_left() const;

  [[nodiscard]] orientation
  find_right() const;

  [[nodiscard]] orientation
  find_back() const;

private:
  [[nodiscard]] bool
  change_side(hexagon_side::orientation destination);

private:
  hexagon _hexagon = hexagon::A;
  orientation _side = orientation::NONE;
};

std::string to_string(hexagon_side::orientation orientation);
std::string to_string(hexagon_side::hexagon hexagon);

}// namespace fys::arena

std::ostream&
operator<<(std::ostream& os, fys::arena::hexagon_side::orientation orientation);

std::ostream&
operator<<(std::ostream& os, fys::arena::hexagon_side::hexagon hexagon);

std::ostream&
operator<<(std::ostream& os, fys::arena::hexagon_side hexagon_side);

#endif// !FYS_HEXAGONSIDE_HH
