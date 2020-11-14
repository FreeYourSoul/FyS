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
struct hexa_side_orient {
  constexpr static bool
  is_movable([[maybe_unused]] fys::arena::hexagon_side::orientation dest) { return false; }
};

template<>
struct hexa_side_orient<HexaA, North> {
  constexpr static bool
  is_movable(fys::arena::hexagon_side::orientation dest) { return dest == h::A_NE || dest == h::A_NW; }
};

template<>
struct hexa_side_orient<HexaA, South> {
  constexpr static bool
  is_movable(fys::arena::hexagon_side::orientation dest) {
    return dest == h::A_SE || dest == h::A_SW || dest == h::C_NW || dest == h::C_SW;
  }
};

template<>
struct hexa_side_orient<HexaA, NorthEast> {
  constexpr static bool
  is_movable(fys::arena::hexagon_side::orientation dest) {
    return dest == h::A_N || dest == h::A_SE || dest == h::B_S || dest == h::B_NW || dest == h::B_SW || dest == h::C_N || dest == h::C_NW;
  }
};

template<>
struct hexa_side_orient<HexaA, NorthWest> {
  constexpr static bool
  is_movable(fys::arena::hexagon_side::orientation dest) { return dest == h::A_N || dest == h::A_SW; }
};

template<>// A_SE
struct hexa_side_orient<HexaA, SouthEast> {
  constexpr static bool
  is_movable(fys::arena::hexagon_side::orientation dest) {
    return dest == h::A_NE || dest == h::A_S || dest == h::B_S || dest == h::B_SW || dest == h::C_N || dest == h::C_NW || dest == h::C_SW;
  }
};

template<>// A_SW
struct hexa_side_orient<HexaA, SouthWest> {
  constexpr static bool
  is_movable(fys::arena::hexagon_side::orientation dest) { return dest == h::A_NW || dest == h::A_S; }
};

template<>
struct hexa_side_orient<HexaB, North> {
  constexpr static bool
  is_movable(fys::arena::hexagon_side::orientation dest) { return dest == h::B_NW || dest == h::B_NE; }
};

template<>
struct hexa_side_orient<HexaB, NorthEast> {
  constexpr static bool
  is_movable(fys::arena::hexagon_side::orientation dest) { return dest == h::B_N || dest == h::B_SE; }
};

template<>
struct hexa_side_orient<HexaB, NorthWest> {
  constexpr static bool
  is_movable(fys::arena::hexagon_side::orientation dest) {
    return dest == h::B_N || dest == h::B_SW || dest == h::A_N || dest == h::A_NE;
  }
};

template<>
struct hexa_side_orient<HexaB, South> {
  constexpr static bool
  is_movable(fys::arena::hexagon_side::orientation dest) {
    return dest == h::B_SW || dest == h::B_SE || dest == h::C_N || dest == h::C_NE || dest == h::C_NW || dest == h::A_NE || dest == h::A_SE;
  }
};

template<>
struct hexa_side_orient<HexaB, SouthEast> {
  constexpr static bool
  is_movable(fys::arena::hexagon_side::orientation dest) {
    return dest == h::B_S || dest == h::B_NE || dest == h::C_N || dest == h::C_NE;
  }
};

template<>
struct hexa_side_orient<HexaB, SouthWest> {
  constexpr static bool
  is_movable(fys::arena::hexagon_side::orientation dest) {
    return dest == h::B_NW || dest == h::B_S || dest == h::C_N || dest == h::C_NW || dest == h::A_NE || dest == h::A_SE || dest == h::A_N;
  }
};

template<>
struct hexa_side_orient<HexaC, North> {
  constexpr static bool
  is_movable(fys::arena::hexagon_side::orientation dest) {
    return dest == h::C_NE || dest == h::C_NW || dest == h::B_S || dest == h::B_SE || dest == h::B_SW || dest == h::A_NE || dest == h::A_SE;
  }
};

template<>
struct hexa_side_orient<HexaC, NorthEast> {
  constexpr static bool
  is_movable(fys::arena::hexagon_side::orientation dest) {
    return dest == h::C_N || dest == h::C_SE || dest == h::B_S || dest == h::B_SE;
  }
};

template<>
struct hexa_side_orient<HexaC, NorthWest> {
  constexpr static bool
  is_movable(fys::arena::hexagon_side::orientation dest) {
    return dest == h::C_N || dest == h::C_SW || dest == h::A_SE || dest == h::A_S || dest == h::A_NE || dest == h::B_S || dest == h::B_SW;
  }
};

template<>
struct hexa_side_orient<HexaC, South> {
  constexpr static bool
  is_movable(fys::arena::hexagon_side::orientation dest) { return dest == h::C_SW || dest == h::C_SE; }
};

template<>
struct hexa_side_orient<HexaC, SouthWest> {
  constexpr static bool
  is_movable(fys::arena::hexagon_side::orientation dest) {
    return dest == h::C_S || dest == h::C_NW || dest == h::A_S || dest == h::A_SE;
  }
};

template<>
struct hexa_side_orient<HexaC, SouthEast> {
  constexpr static bool
  is_movable(fys::arena::hexagon_side::orientation dest) { return dest == h::C_S || dest == h::C_NE; }
};

// Specialization to check the hexagon
template<typename Hexa>
struct HexaSide;

template<>
struct HexaSide<HexaA> {
  constexpr static bool
  is_movable(fys::arena::hexagon_side::orientation origin, fys::arena::hexagon_side::orientation dest) {
    if (origin == dest)
      return false;
    if (origin == fys::arena::hexagon_side::orientation::A_S)
      return hexa_side_orient<HexaA, South>::is_movable(dest);
    else if (origin == fys::arena::hexagon_side::orientation::A_SE)
      return hexa_side_orient<HexaA, SouthEast>::is_movable(dest);
    else if (origin == fys::arena::hexagon_side::orientation::A_SW)
      return hexa_side_orient<HexaA, SouthWest>::is_movable(dest);
    else if (origin == fys::arena::hexagon_side::orientation::A_N)
      return hexa_side_orient<HexaA, North>::is_movable(dest);
    else if (origin == fys::arena::hexagon_side::orientation::A_NE)
      return hexa_side_orient<HexaA, NorthEast>::is_movable(dest);
    else if (origin == fys::arena::hexagon_side::orientation::A_NW)
      return hexa_side_orient<HexaA, NorthWest>::is_movable(dest);
    return false;
  }
};

template<>
struct HexaSide<HexaB> {
  constexpr static bool
  is_movable(fys::arena::hexagon_side::orientation origin, fys::arena::hexagon_side::orientation dest) {
    if (origin == dest)
      return false;
    if (origin == fys::arena::hexagon_side::orientation::B_S)
      return hexa_side_orient<HexaB, South>::is_movable(dest);
    else if (origin == fys::arena::hexagon_side::orientation::B_SE)
      return hexa_side_orient<HexaB, SouthEast>::is_movable(dest);
    else if (origin == fys::arena::hexagon_side::orientation::B_SW)
      return hexa_side_orient<HexaB, SouthWest>::is_movable(dest);
    if (origin == fys::arena::hexagon_side::orientation::B_N)
      return hexa_side_orient<HexaB, North>::is_movable(dest);
    else if (origin == fys::arena::hexagon_side::orientation::B_NE)
      return hexa_side_orient<HexaB, NorthEast>::is_movable(dest);
    else if (origin == fys::arena::hexagon_side::orientation::B_NW)
      return hexa_side_orient<HexaB, NorthWest>::is_movable(dest);
    return false;
  }
};

template<>
struct HexaSide<HexaC> {
  constexpr static bool
  is_movable(fys::arena::hexagon_side::orientation origin, fys::arena::hexagon_side::orientation dest) {
    if (origin == dest)
      return false;
    if (origin == fys::arena::hexagon_side::orientation::C_S)
      return hexa_side_orient<HexaC, South>::is_movable(dest);
    else if (origin == fys::arena::hexagon_side::orientation::C_SE)
      return hexa_side_orient<HexaC, SouthEast>::is_movable(dest);
    else if (origin == fys::arena::hexagon_side::orientation::C_SW)
      return hexa_side_orient<HexaC, SouthWest>::is_movable(dest);
    if (origin == fys::arena::hexagon_side::orientation::C_N)
      return hexa_side_orient<HexaC, North>::is_movable(dest);
    else if (origin == fys::arena::hexagon_side::orientation::C_NE)
      return hexa_side_orient<HexaC, NorthEast>::is_movable(dest);
    else if (origin == fys::arena::hexagon_side::orientation::C_NW)
      return hexa_side_orient<HexaC, NorthWest>::is_movable(dest);
    return false;
  }
};

}// namespace

namespace fys::arena {

bool hexagon_side::move(hexagon_side::orientation destination_side, bool force_movement) {
  if (force_movement || (destination_side != hexagon_side::orientation::NONE && can_move_to(destination_side))) {
    return change_side(destination_side);
  }
  return false;
}

bool hexagon_side::change_side(hexagon_side::orientation destination) {
  switch (destination) {
  case hexagon_side::orientation::A_N:
  case hexagon_side::orientation::A_NE:
  case hexagon_side::orientation::A_NW:
  case hexagon_side::orientation::A_S:
  case hexagon_side::orientation::A_SE:
  case hexagon_side::orientation::A_SW:
    _hexagon = hexagon_side::hexagon::A;
    _side = destination;
    return true;
  case hexagon_side::orientation::B_N:
  case hexagon_side::orientation::B_NE:
  case hexagon_side::orientation::B_NW:
  case hexagon_side::orientation::B_S:
  case hexagon_side::orientation::B_SE:
  case hexagon_side::orientation::B_SW:
    _hexagon = hexagon_side::hexagon::B;
    _side = destination;
    return true;
  case hexagon_side::orientation::C_N:
  case hexagon_side::orientation::C_NE:
  case hexagon_side::orientation::C_NW:
  case hexagon_side::orientation::C_S:
  case hexagon_side::orientation::C_SE:
  case hexagon_side::orientation::C_SW:
    _hexagon = hexagon_side::hexagon::C;
    _side = destination;
    return true;
  default: return false;
  }
  return false;
}

hexagon_side::orientation
hexagon_side::find_left() const {
  if (_side == hexagon_side::orientation::A_N)
    return hexagon_side::orientation::A_NW;
  else if (_side == hexagon_side::orientation::B_N)
    return hexagon_side::orientation::B_NW;
  else if (_side == hexagon_side::orientation::C_N)
    return hexagon_side::orientation::C_NW;
  return static_cast<hexagon_side::orientation>(static_cast<int>(_side) - 1);
}

hexagon_side::orientation
hexagon_side::find_right() const {
  if (_side == hexagon_side::orientation::A_NW)
    return hexagon_side::orientation::A_N;
  else if (_side == hexagon_side::orientation::B_NW)
    return hexagon_side::orientation::B_N;
  else if (_side == hexagon_side::orientation::C_NW)
    return hexagon_side::orientation::C_N;
  return static_cast<hexagon_side::orientation>(static_cast<int>(_side) + 1);
}

hexagon_side::orientation
hexagon_side::find_back() const {
  if (!is_ambush_side())
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

bool hexagon_side::is_ambush_side() const {
  return _side == orientation::A_NE || _side == orientation::A_SE || _side == orientation::B_S || _side == orientation::B_SW || _side == orientation::C_N || _side == orientation::C_NW;
}

bool hexagon_side::can_move_to(hexagon_side::orientation destination_side) const {
  if (_hexagon == hexagon::A)
    return HexaSide<HexaA>::is_movable(_side, destination_side);
  if (_hexagon == hexagon::B)
    return HexaSide<HexaB>::is_movable(_side, destination_side);
  if (_hexagon == hexagon::C)
    return HexaSide<HexaC>::is_movable(_side, destination_side);
  return false;
}

std::string to_string(hexagon_side::orientation orientation) {
  switch (orientation) {
  case fys::arena::hexagon_side::orientation::A_N: return "A_N";
  case fys::arena::hexagon_side::orientation::B_N: return "B_N";
  case fys::arena::hexagon_side::orientation::C_N: return "C_N";
  case fys::arena::hexagon_side::orientation::A_S: return "A_S";
  case fys::arena::hexagon_side::orientation::B_S: return "B_S";
  case fys::arena::hexagon_side::orientation::C_S: return "C_S";
  case fys::arena::hexagon_side::orientation::A_NE: return "A_NE";
  case fys::arena::hexagon_side::orientation::B_NE: return "B_NE";
  case fys::arena::hexagon_side::orientation::C_NE: return "C_NE";
  case fys::arena::hexagon_side::orientation::A_NW: return "A_NW";
  case fys::arena::hexagon_side::orientation::B_NW: return "B_NW";
  case fys::arena::hexagon_side::orientation::C_NW: return "C_NW";
  case fys::arena::hexagon_side::orientation::A_SE: return "A_SE";
  case fys::arena::hexagon_side::orientation::B_SE: return "B_SE";
  case fys::arena::hexagon_side::orientation::C_SE: return "C_SE";
  case fys::arena::hexagon_side::orientation::A_SW: return "A_SW";
  case fys::arena::hexagon_side::orientation::B_SW: return "B_SW";
  case fys::arena::hexagon_side::orientation::C_SW: return "C_SW";
  case fys::arena::hexagon_side::orientation::NONE: return "NONE(ERROR)";
  }
}

std::string to_string(hexagon_side::hexagon hexagon) {
  switch (hexagon) {
  case fys::arena::hexagon_side::hexagon::A: return "A";
  case fys::arena::hexagon_side::hexagon::B: return "B";
  case fys::arena::hexagon_side::hexagon::C: return "C";
  }
}

}// namespace fys::arena

std::ostream&
operator<<(std::ostream& os, fys::arena::hexagon_side::orientation orientation) {
  switch (orientation) {
  case fys::arena::hexagon_side::orientation::A_N:
  case fys::arena::hexagon_side::orientation::B_N:
  case fys::arena::hexagon_side::orientation::C_N: return os << "[North]";
  case fys::arena::hexagon_side::orientation::A_S:
  case fys::arena::hexagon_side::orientation::B_S:
  case fys::arena::hexagon_side::orientation::C_S: return os << "[South]";
  case fys::arena::hexagon_side::orientation::A_NE:
  case fys::arena::hexagon_side::orientation::B_NE:
  case fys::arena::hexagon_side::orientation::C_NE: return os << "[NorthEast]";
  case fys::arena::hexagon_side::orientation::A_NW:
  case fys::arena::hexagon_side::orientation::B_NW:
  case fys::arena::hexagon_side::orientation::C_NW: return os << "[NorthWest]";
  case fys::arena::hexagon_side::orientation::A_SE:
  case fys::arena::hexagon_side::orientation::B_SE:
  case fys::arena::hexagon_side::orientation::C_SE: return os << "[SouthEast]";
  case fys::arena::hexagon_side::orientation::A_SW:
  case fys::arena::hexagon_side::orientation::B_SW:
  case fys::arena::hexagon_side::orientation::C_SW: return os << "[SouthWest]";
  case fys::arena::hexagon_side::orientation::NONE: return os << "[NONE(ERROR)]";
  }
  return os;
}

std::ostream&
operator<<(std::ostream& os, fys::arena::hexagon_side::hexagon hexagon) {
  switch (hexagon) {
  case fys::arena::hexagon_side::hexagon::A: return os << "[A]";
  case fys::arena::hexagon_side::hexagon::B: return os << "[B]";
  case fys::arena::hexagon_side::hexagon::C: return os << "[C]";
  }
  return os;
}

std::ostream&
operator<<(std::ostream& os, fys::arena::hexagon_side hexagon_side) {
  auto hs = *hexagon_side;
  return os << "hexagonSide:" << hs.first << hs.second;
}