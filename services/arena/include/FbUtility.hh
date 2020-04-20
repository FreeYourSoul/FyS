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


#ifndef FYS_ONLINE_FBUTILITY_HH
#define FYS_ONLINE_FBUTILITY_HH

#include <spdlog/spdlog.h>

namespace fys::util {

using namespace fys::arena;
using namespace fys::fb;

[[nodiscard]] static SideOrient
convertArenaOrientToFb(HexagonSide::Orientation orient)
{
	switch (orient) {
		case HexagonSide::Orientation::A_N: return SideOrient_A_N;
		case HexagonSide::Orientation::A_NE: return SideOrient_A_NE;
		case HexagonSide::Orientation::A_NW: return SideOrient_A_NW;
		case HexagonSide::Orientation::A_S: return SideOrient_A_S;
		case HexagonSide::Orientation::A_SE: return SideOrient_A_SE;
		case HexagonSide::Orientation::A_SW: return SideOrient_A_SW;

		case HexagonSide::Orientation::B_N: return SideOrient_B_N;
		case HexagonSide::Orientation::B_NE: return SideOrient_B_NE;
		case HexagonSide::Orientation::B_NW: return SideOrient_B_NW;
		case HexagonSide::Orientation::B_S: return SideOrient_B_S;
		case HexagonSide::Orientation::B_SE: return SideOrient_B_SE;
		case HexagonSide::Orientation::B_SW: return SideOrient_B_SW;

		case HexagonSide::Orientation::C_N: return SideOrient_C_N;
		case HexagonSide::Orientation::C_NE: return SideOrient_C_NE;
		case HexagonSide::Orientation::C_NW: return SideOrient_C_NW;
		case HexagonSide::Orientation::C_S: return SideOrient_C_S;
		case HexagonSide::Orientation::C_SE: return SideOrient_C_SE;
		case HexagonSide::Orientation::C_SW: return SideOrient_C_SW;

		case HexagonSide::Orientation::NONE: break;
	}
	SPDLOG_ERROR("Trying to convert Orientation::NONE, value replaced by {}", SideOrient_MIN);
	return SideOrient_MIN;
}

[[nodiscard]] static HexagonSide::Orientation
convertFbOrientToArena(SideOrient orient)
{
	switch (orient) {
		case SideOrient_A_N: return HexagonSide::Orientation::A_N;
		case SideOrient_A_NE: return HexagonSide::Orientation::A_NE;
		case SideOrient_A_NW: return HexagonSide::Orientation::A_NW;
		case SideOrient_A_S: return HexagonSide::Orientation::A_S;
		case SideOrient_A_SE: return HexagonSide::Orientation::A_SE;
		case SideOrient_A_SW: return HexagonSide::Orientation::A_SW;

		case SideOrient_B_N: return HexagonSide::Orientation::B_N;
		case SideOrient_B_NE: return HexagonSide::Orientation::B_NE;
		case SideOrient_B_NW: return HexagonSide::Orientation::B_NW;
		case SideOrient_B_S: return HexagonSide::Orientation::B_S;
		case SideOrient_B_SE: return HexagonSide::Orientation::B_SE;
		case SideOrient_B_SW: return HexagonSide::Orientation::B_SW;

		case SideOrient_C_N: return HexagonSide::Orientation::C_N;
		case SideOrient_C_NE: return HexagonSide::Orientation::C_NE;
		case SideOrient_C_NW: return HexagonSide::Orientation::C_NW;
		case SideOrient_C_S: return HexagonSide::Orientation::C_S;
		case SideOrient_C_SE: return HexagonSide::Orientation::C_SE;
		case SideOrient_C_SW: return HexagonSide::Orientation::C_SW;
	}
	return HexagonSide::Orientation::NONE;
}

}

#endif //FYS_ONLINE_FBUTILITY_HH
