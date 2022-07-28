// MIT License
//
// Copyright (c) 2021-2022 Quentin Balland
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

#ifndef FYS_ONLINE_FBUTILITY_HH
#define FYS_ONLINE_FBUTILITY_HH

#include <fmt/format.h>
#include <logger.hh>

namespace fys::util {

using namespace fys::arena;
using namespace fys::fb::arn;

[[nodiscard]] static SideOrient
convert_arena_orient_to_fb(hexagon_side::orientation orient) {
  switch (orient) {
  case hexagon_side::orientation::A_N: return SideOrient_A_N;
  case hexagon_side::orientation::A_NE: return SideOrient_A_NE;
  case hexagon_side::orientation::A_NW: return SideOrient_A_NW;
  case hexagon_side::orientation::A_S: return SideOrient_A_S;
  case hexagon_side::orientation::A_SE: return SideOrient_A_SE;
  case hexagon_side::orientation::A_SW: return SideOrient_A_SW;

  case hexagon_side::orientation::B_N: return SideOrient_B_N;
  case hexagon_side::orientation::B_NE: return SideOrient_B_NE;
  case hexagon_side::orientation::B_NW: return SideOrient_B_NW;
  case hexagon_side::orientation::B_S: return SideOrient_B_S;
  case hexagon_side::orientation::B_SE: return SideOrient_B_SE;
  case hexagon_side::orientation::B_SW: return SideOrient_B_SW;

  case hexagon_side::orientation::C_N: return SideOrient_C_N;
  case hexagon_side::orientation::C_NE: return SideOrient_C_NE;
  case hexagon_side::orientation::C_NW: return SideOrient_C_NW;
  case hexagon_side::orientation::C_S: return SideOrient_C_S;
  case hexagon_side::orientation::C_SE: return SideOrient_C_SE;
  case hexagon_side::orientation::C_SW: return SideOrient_C_SW;

  case hexagon_side::orientation::NONE: break;
  }
  log_error(fmt::format("Trying to convert Orientation::NONE, value replaced by {}", SideOrient_MIN));
  return SideOrient_MIN;
}

[[nodiscard]] static hexagon_side::orientation
convertFbOrientToArena(SideOrient orient) {
  switch (orient) {
  case SideOrient_A_N: return hexagon_side::orientation::A_N;
  case SideOrient_A_NE: return hexagon_side::orientation::A_NE;
  case SideOrient_A_NW: return hexagon_side::orientation::A_NW;
  case SideOrient_A_S: return hexagon_side::orientation::A_S;
  case SideOrient_A_SE: return hexagon_side::orientation::A_SE;
  case SideOrient_A_SW: return hexagon_side::orientation::A_SW;

  case SideOrient_B_N: return hexagon_side::orientation::B_N;
  case SideOrient_B_NE: return hexagon_side::orientation::B_NE;
  case SideOrient_B_NW: return hexagon_side::orientation::B_NW;
  case SideOrient_B_S: return hexagon_side::orientation::B_S;
  case SideOrient_B_SE: return hexagon_side::orientation::B_SE;
  case SideOrient_B_SW: return hexagon_side::orientation::B_SW;

  case SideOrient_C_N: return hexagon_side::orientation::C_N;
  case SideOrient_C_NE: return hexagon_side::orientation::C_NE;
  case SideOrient_C_NW: return hexagon_side::orientation::C_NW;
  case SideOrient_C_S: return hexagon_side::orientation::C_S;
  case SideOrient_C_SE: return hexagon_side::orientation::C_SE;
  case SideOrient_C_SW: return hexagon_side::orientation::C_SW;
  }
  return hexagon_side::orientation::NONE;
}

}// namespace fys::util

#endif//FYS_ONLINE_FBUTILITY_HH
