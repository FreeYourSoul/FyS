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

#include <catch2/catch.hpp>
#include <fightingPit/HexagonSide.hh>

TEST_CASE("HexagonSide test", "[service][arena]")
{

	SECTION("test ambush side") {

		SECTION("Not Ambush : A_N") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_N);
			REQUIRE_FALSE(hexagon.isAmbushSide());

		} // End section : Not Ambush : A_N

		SECTION("Not Ambush : A_NW") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_NW);
			REQUIRE_FALSE(hexagon.isAmbushSide());

		} // End section : Not Ambush : A_NW

		SECTION("Not Ambush : A_SW") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_SW);
			REQUIRE_FALSE(hexagon.isAmbushSide());

		} // End section : Not Ambush : A_SW

		SECTION("Not Ambush : A_S") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_S);
			REQUIRE_FALSE(hexagon.isAmbushSide());

		} // End section : Not Ambush : A_S

		SECTION("Not Ambush : B_N") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_N);
			REQUIRE_FALSE(hexagon.isAmbushSide());

		} // End section : Not Ambush : B_N

		SECTION("Not Ambush : B_NE") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_NE);
			REQUIRE_FALSE(hexagon.isAmbushSide());

		} // End section : Not Ambush : B_NE

		SECTION("Not Ambush : B_NW") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_NW);
			REQUIRE_FALSE(hexagon.isAmbushSide());

		} // End section : Not Ambush : B_NW

		SECTION("Not Ambush : B_SE") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SE);
			REQUIRE_FALSE(hexagon.isAmbushSide());

		} // End section : Not Ambush : B_SE

		SECTION("Not Ambush : C_S") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_S);
			REQUIRE_FALSE(hexagon.isAmbushSide());

		} // End section : Not Ambush : C_S

		SECTION("Not Ambush : C_SW") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_SW);
			REQUIRE_FALSE(hexagon.isAmbushSide());

		} // End section : Not Ambush : C_SW

		SECTION("Not Ambush : C_SE") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_SE);
			REQUIRE_FALSE(hexagon.isAmbushSide());

		} // End section : Not Ambush : C_SE

		SECTION("Not Ambush : C_NE") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_NE);
			REQUIRE_FALSE(hexagon.isAmbushSide());

		} // End section : Not Ambush : C_NE

		SECTION("Ambush : A_NE") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_NE);
			REQUIRE(hexagon.isAmbushSide());

		} // End section : Ambush : A_NE

		SECTION("Ambush : A_SE") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_SE);
			REQUIRE(hexagon.isAmbushSide());

		} // End section : Ambush : A_SE

		SECTION("Ambush : B_SW") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SW);
			REQUIRE(hexagon.isAmbushSide());

		} // End section : Ambush : B_SW

		SECTION("Ambush : B_S") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_S);
			REQUIRE(hexagon.isAmbushSide());

		} // End section : Ambush : B_S

		SECTION("Ambush : C_N") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_N);
			REQUIRE(hexagon.isAmbushSide());

		} // End section : Ambush : C_N

		SECTION("Ambush : C_NW") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_NW);
			REQUIRE(hexagon.isAmbushSide());

		} // End section : Ambush : C_NW

	} // End section : test ambush side

	SECTION("test Move") {

		SECTION("Force move") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_N);

			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_N) == *hexagon);
			REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_S));

			REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::B_S, true));
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_S) == *hexagon);

			REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_S));
			REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_S, true));
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_S) == *hexagon);

		} // End section : Force move

		SECTION("Hexagon A : A_N") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_N);

			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_N) == *hexagon);

			SECTION("Not possible") {
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_SW));
			} // End section : Not possible

			SECTION("to A_NE") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NE));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_NE) == *hexagon);
			} // End section : toNE

			SECTION("to A_NW") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NW));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_NW) == *hexagon);
			} // End section : toNW

		} // End section : Hexagon A

		SECTION("Hexagon A : A_NE") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_NE);

			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_NE) == *hexagon);

			SECTION("Not possible") {
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_SW));

			} // End section : Not possible

			SECTION("to A_N") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::A_N));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_N) == *hexagon);
			} // End section : toN

			SECTION("to A_SE") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SE));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_SE) == *hexagon);
			} // End section : toSE

			SECTION("to B_NW") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NW));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_NW) == *hexagon);
			} // End section : to B_NW

			SECTION("to B_S") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::B_S));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_S) == *hexagon);
			} // End section : to B_S

			SECTION("to B_SW") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SW));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SW) == *hexagon);
			} // End section : to B_SW

			SECTION("to C_N") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_N));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_N) == *hexagon);
			} // End section : to C_N

			SECTION("to C_NW") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NW));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_NW) == *hexagon);
			} // End section : to C_NW

		} // End section : Hexagon A

		SECTION("Hexagon A : A_NW") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_NW);

			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_NW) == *hexagon);

			SECTION("Not Possible") {
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_SW));

			} // End section : Not Possible

			SECTION("to A_N") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::A_N));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_N) == *hexagon);

			} // End section : to A_N

			SECTION("to A_SW") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SW));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_SW) == *hexagon);

			} // End section : to A_SW

		} // End section : Hexagon A

		SECTION("Hexagon A : A_S") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_S);

			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_S) == *hexagon);

			SECTION("Not Possible") {
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));

			} // End section : Not Possible

			SECTION("to A_SW") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SW));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_SW) == *hexagon);

			} // End section : to A_SW

			SECTION("to A_SE") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SE));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_SE) == *hexagon);

			} // End section : to A_SE

			SECTION("to C_NW") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NW));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_NW) == *hexagon);

			} // End section : to C_NW

			SECTION("to C_SW") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_SW));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_SW) == *hexagon);

			} // End section : C_SW

		} // End section : Hexagon A : A_S

		SECTION("Hexagon A : A_SE") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_SE);

			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_SE) == *hexagon);

			SECTION("Not Possible") {
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));

			} // End section : Not Possible

			SECTION("to A_NE") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NE));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_NE) == *hexagon);

			} // End section : to A_NE

			SECTION("to A_S") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::A_S));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_S) == *hexagon);

			} // End section : to A_S

			SECTION("to B_S") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::B_S));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_S) == *hexagon);

			} // End section : to B_S

			SECTION("to B_SW") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SW));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SW) == *hexagon);

			} // End section : to B_SW

			SECTION("to C_N") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_N));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_N) == *hexagon);

			} // End section : to C_N

			SECTION("to C_NW") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NW));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_NW) == *hexagon);

			} // End section : to C_NW

			SECTION("to C_SW") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_SW));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_SW) == *hexagon);

			} // End section : to C_SW


		} // End section : Hexagon A : A_SE

		SECTION("Hexagon A : A_SW") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_SW);

			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_SW) == *hexagon);

			SECTION("Not Possible") {
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_SW));

			} // End section : Not Possible

			SECTION("to A_NW") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NW));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_NW) == *hexagon);

			} // End section : to A_NW

			SECTION("to A_S") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::A_S));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_S) == *hexagon);

			} // End section : to A_S

		} // End section : Hexagon A : A_SW

		SECTION("Impossible Position") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SW);
			REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::NONE));
		} // End section

		SECTION("Hexagon B : B_N") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_N);

			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_N) == *hexagon);

			SECTION("Not Possible") {
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_SW));

			} // End section : Not Possible

			SECTION("to B_NE") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NE));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_NE) == *hexagon);

			} // End section : to B_NE

			SECTION("to B_NW") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NW));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_NW) == *hexagon);

			} // End section : to B_NW

		} // End section : Hexagon B : B_N

		SECTION("Hexagon B : B_NE") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_NE);

			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_NE) == *hexagon);

			SECTION("Not Possible") {
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_SW));

			} // End section : Not Possible

			SECTION("to B_N") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::B_N));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_N) == *hexagon);

			} // End section : to B_N

			SECTION("to B_SE") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SE));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SE) == *hexagon);

			} // End section : to B_SE

		} // End section : Hexagon B : B_NE

		SECTION("Hexagon B : B_NW") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_NW);

			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_NW) == *hexagon);

			SECTION("Not Possible") {
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_SW));

			} // End section : Not Possible

			SECTION("to B_N") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::B_N));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_N) == *hexagon);

			} // End section : to B_N

			SECTION("to B_SW") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SW));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SW) == *hexagon);

			} // End section : to B_SW

			SECTION("to A_N") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::A_N));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_N) == *hexagon);

			} // End section : to A_N

			SECTION("to A_NE") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NE));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_NE) == *hexagon);

			} // End section : to A_NE

		} // End section : Hexagon B : B_NW

		SECTION("Hexagon B : B_S") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_S);

			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_S) == *hexagon);

			SECTION("Not Possible") {
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_SW));

			} // End section : Not Possible

			SECTION("to B_SE") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SE));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SE) == *hexagon);

			} // End section : to B_SE

			SECTION("to B_SW") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SW));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SW) == *hexagon);

			} // End section : to B_SW

			SECTION("to C_N") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_N));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_N) == *hexagon);

			} // End section : to C_N

			SECTION("to C_NE") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_NE) == *hexagon);

			} // End section : to C_NE

			SECTION("to C_NW") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NW));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_NW) == *hexagon);

			} // End section : to C_NW

			SECTION("to A_SE") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SE));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_SE) == *hexagon);

			} // End section : to A_SE

			SECTION("to A_NE") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NE));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_NE) == *hexagon);

			} // End section : to A_NE

		} // End section : Hexagon B : B_S

		SECTION("Hexagon B : B_SW") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SW);

			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SW) == *hexagon);

			SECTION("Not Possible") {
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_SW));

			} // End section : Not Possible

			SECTION("to B_S") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::B_S));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_S) == *hexagon);

			} // End section : to B_S

			SECTION("to B_NW") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NW));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_NW) == *hexagon);

			} // End section : to B_NW

			SECTION("to A_N") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::A_N));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_N) == *hexagon);

			} // End section : to A_N

			SECTION("to A_NE") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NE));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_NE) == *hexagon);

			} // End section : to A_NE

			SECTION("to A_SE") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SE));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_SE) == *hexagon);

			} // End section : to A_SE

			SECTION("to C_N") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_N));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_N) == *hexagon);

			} // End section : to C_N

			SECTION("to C_NW") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NW));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_NW) == *hexagon);

			} // End section : to C_NW

		} // End section : Hexagon B : B_SW

		SECTION("Hexagon B : B_SE") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SE);

			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SE) == *hexagon);

			SECTION("Not Possible") {
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_SW));

			} // End section : Not Possible

			SECTION("to B_S") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::B_S));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_S) == *hexagon);

			} // End section : to B_S

			SECTION("to B_NE") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NE));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_NE) == *hexagon);

			} // End section : to B_NE

			SECTION("to C_N") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_N));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_N) == *hexagon);

			} // End section : to C_N

			SECTION("to C_NE") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_NE) == *hexagon);

			} // End section : to C_NE

		} // End section : Hexagon B : B_SW

		SECTION("Hexagon C : C_N") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_N);

			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_N) == *hexagon);

			SECTION("Not Possible") {
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_SW));

			} // End section : Not Possible

			SECTION("to C_NE") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_NE) == *hexagon);

			} // End section : to C_NE

			SECTION("to C_NW") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NW));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_NW) == *hexagon);

			} // End section : to C_NW

			SECTION("to B_S") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::B_S));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_S) == *hexagon);

			} // End section : to B_S

			SECTION("to B_SE") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SE));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SE) == *hexagon);

			} // End section : to B_SE

			SECTION("to B_SW") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SW));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SW) == *hexagon);

			} // End section : to B_SW

			SECTION("to A_NE") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NE));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_NE) == *hexagon);

			} // End section : to A_NE

			SECTION("to A_SE") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SE));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_SE) == *hexagon);

			} // End section : to A_SE

		} // End section : Hexagon C : C_N

		SECTION("Hexagon C : C_NE") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_NE);

			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_NE) == *hexagon);

			SECTION("Not Possible") {
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_SW));

			} // End section : Not Possible

			SECTION("to C_N") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_N));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_N) == *hexagon);

			} // End section : to C_N

			SECTION("to C_SE") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_SE));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_SE) == *hexagon);

			} // End section : to C_SE

			SECTION("to B_S") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::B_S));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_S) == *hexagon);

			} // End section : to B_S

			SECTION("to B_SE") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SE));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SE) == *hexagon);

			} // End section : to B_SE

		} // End section : Hexagon C : C_NE

		SECTION("Hexagon C : C_NW") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_NW);

			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_NW) == *hexagon);

			SECTION("Not Possible") {
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_S));

			} // End section : Not Possible

			SECTION("to C_N") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_N));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_N) == *hexagon);

			} // End section : to C_N

			SECTION("to C_SW") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_SW));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_SW) == *hexagon);

			} // End section : to to C_SW

			SECTION("to A_S") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::A_S));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_S) == *hexagon);

			} // End section : to A_S

			SECTION("to A_SE") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SE));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_SE) == *hexagon);

			} // End section : to A_SE

			SECTION("to A_NE") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NE));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_NE) == *hexagon);

			} // End section : to A_NE

			SECTION("to B_S") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::B_S));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_S) == *hexagon);

			} // End section : to C_S

			SECTION("to B_SW") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SW));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SW) == *hexagon);

			} // End section : to C_SW

		} // End section : Hexagon C : C_NW

		SECTION("Hexagon C : C_S") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_S);

			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_S) == *hexagon);

			SECTION("Not Possible") {
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_S));

			} // End section : Not Possible

			SECTION("to C_SW") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_SW));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_SW) == *hexagon);

			} // End section : to C_SW

			SECTION("to C_SE") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_SE));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_SE) == *hexagon);

			} // End section : to C_SE

		} // End section : Hexagon C : C_S

		SECTION("Hexagon C : C_SE") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_SE);

			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_SE) == *hexagon);

			SECTION("Not Possible") {
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_SW));

			} // End section : Not Possible

			SECTION("to C_S") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_S));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_S) == *hexagon);

			} // End section : to C_S

			SECTION("to C_NE") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_NE) == *hexagon);

			} // End section : to C_NE

		} // End section : Hexagon C : C_SE

		SECTION("Hexagon C : C_SW") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_SW);

			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_SW) == *hexagon);

			SECTION("Not Possible") {
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_N));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_SE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::A_NE));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::C_SW));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_S));
				REQUIRE_FALSE(hexagon.move(fys::arena::HexagonSide::Orientation::B_SW));

			} // End section : Not Possible

			SECTION("to C_S") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_S));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_S) == *hexagon);

			} // End section : to C_S

			SECTION("to C_NW") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::C_NW));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_NW) == *hexagon);

			} // End section : to C_NW

			SECTION("to A_S") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::A_S));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_S) == *hexagon);

			} // End section : to A_S

			SECTION("to A_SE") {
				REQUIRE(hexagon.move(fys::arena::HexagonSide::Orientation::A_SE));
				REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_SE) == *hexagon);

			} // End section : to A_SE

		} // End section : Hexagon C : C_SW

	} // End section : test Move

	SECTION("test MoveLeft") {

		SECTION("Hexagone A full circle by left") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_SE);

			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_SE) == *hexagon);
			REQUIRE(hexagon.moveLeft());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_NE) == *hexagon);
			REQUIRE(hexagon.moveLeft());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_N) == *hexagon);
			REQUIRE(hexagon.moveLeft());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_NW) == *hexagon);
			REQUIRE(hexagon.moveLeft());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_SW) == *hexagon);
			REQUIRE(hexagon.moveLeft());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_S) == *hexagon);
			REQUIRE(hexagon.moveLeft());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_SE) == *hexagon);
			REQUIRE(hexagon.moveLeft());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_NE) == *hexagon);
			REQUIRE(hexagon.moveLeft());
		} // End section : Hexagone A full circle by left

		SECTION("Hexagone B full circle by left") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SE);

			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SE) == *hexagon);
			REQUIRE(hexagon.moveLeft());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_NE) == *hexagon);
			REQUIRE(hexagon.moveLeft());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_N) == *hexagon);
			REQUIRE(hexagon.moveLeft());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_NW) == *hexagon);
			REQUIRE(hexagon.moveLeft());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SW) == *hexagon);
			REQUIRE(hexagon.moveLeft());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_S) == *hexagon);
			REQUIRE(hexagon.moveLeft());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SE) == *hexagon);
			REQUIRE(hexagon.moveLeft());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_NE) == *hexagon);
			REQUIRE(hexagon.moveLeft());
		} // End section : Hexagone B full circle by left

		SECTION("Hexagone C full circle by left") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_SE);

			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_SE) == *hexagon);
			REQUIRE(hexagon.moveLeft());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_NE) == *hexagon);
			REQUIRE(hexagon.moveLeft());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_N) == *hexagon);
			REQUIRE(hexagon.moveLeft());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_NW) == *hexagon);
			REQUIRE(hexagon.moveLeft());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_SW) == *hexagon);
			REQUIRE(hexagon.moveLeft());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_S) == *hexagon);
			REQUIRE(hexagon.moveLeft());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_SE) == *hexagon);
			REQUIRE(hexagon.moveLeft());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_NE) == *hexagon);
			REQUIRE(hexagon.moveLeft());
		} // End section : Hexagone C full circle by left

	} // End section : MoveLeft

	SECTION("test MoveRight") {

		SECTION("Hexagone A full circle by right") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_SE);

			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_SE) == *hexagon);
			REQUIRE(hexagon.moveRight());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_S) == *hexagon);
			REQUIRE(hexagon.moveRight());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_SW) == *hexagon);
			REQUIRE(hexagon.moveRight());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_NW) == *hexagon);
			REQUIRE(hexagon.moveRight());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_N) == *hexagon);
			REQUIRE(hexagon.moveRight());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_NE) == *hexagon);
			REQUIRE(hexagon.moveRight());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_SE) == *hexagon);
			REQUIRE(hexagon.moveRight());

		} // End section : Hexagone A full circle by right

		SECTION("Hexagone B full circle by right") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SE);

			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SE) == *hexagon);
			REQUIRE(hexagon.moveRight());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_S) == *hexagon);
			REQUIRE(hexagon.moveRight());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SW) == *hexagon);
			REQUIRE(hexagon.moveRight());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_NW) == *hexagon);
			REQUIRE(hexagon.moveRight());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_N) == *hexagon);
			REQUIRE(hexagon.moveRight());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_NE) == *hexagon);
			REQUIRE(hexagon.moveRight());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SE) == *hexagon);
			REQUIRE(hexagon.moveRight());

		} // End section : Hexagone B full circle by right

		SECTION("Hexagone C full circle by right") {
			fys::arena::HexagonSide hexagon(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_SE);

			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_SE) == *hexagon);
			REQUIRE(hexagon.moveRight());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_S) == *hexagon);
			REQUIRE(hexagon.moveRight());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_SW) == *hexagon);
			REQUIRE(hexagon.moveRight());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_NW) == *hexagon);
			REQUIRE(hexagon.moveRight());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_N) == *hexagon);
			REQUIRE(hexagon.moveRight());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_NE) == *hexagon);
			REQUIRE(hexagon.moveRight());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_SE) == *hexagon);
			REQUIRE(hexagon.moveRight());
		} // End section : Hexagone C full circle by right

	} // End section : MoveRight

	SECTION("test moveBack") {
		fys::arena::HexagonSide hexagonA_S(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_S);
		fys::arena::HexagonSide hexagonA_SE(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_SE);
		fys::arena::HexagonSide hexagonA_SW(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_SW);
		fys::arena::HexagonSide hexagonA_N(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_N);
		fys::arena::HexagonSide hexagonA_NE(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_NE);
		fys::arena::HexagonSide hexagonA_NW(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_NW);

		fys::arena::HexagonSide hexagonB_S(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_S);
		fys::arena::HexagonSide hexagonB_SE(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SE);
		fys::arena::HexagonSide hexagonB_SW(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SW);
		fys::arena::HexagonSide hexagonB_N(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_N);
		fys::arena::HexagonSide hexagonB_NE(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_NE);
		fys::arena::HexagonSide hexagonB_NW(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_NW);

		fys::arena::HexagonSide hexagonC_S(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_S);
		fys::arena::HexagonSide hexagonC_SE(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_SE);
		fys::arena::HexagonSide hexagonC_SW(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_SW);
		fys::arena::HexagonSide hexagonC_N(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_N);
		fys::arena::HexagonSide hexagonC_NE(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_NE);
		fys::arena::HexagonSide hexagonC_NW(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_NW);

		SECTION("All working back") {
			REQUIRE(hexagonA_SE.moveBack());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_NW) == *hexagonA_SE);

			REQUIRE(hexagonA_NE.moveBack());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_SW) == *hexagonA_NE);

			REQUIRE(hexagonB_SW.moveBack());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_NE) == *hexagonB_SW);

			REQUIRE(hexagonB_S.moveBack());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::C, fys::arena::HexagonSide::Orientation::C_N) == *hexagonB_S);

			REQUIRE(hexagonC_N.moveBack());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::B, fys::arena::HexagonSide::Orientation::B_S) == *hexagonC_N);

			REQUIRE(hexagonC_NW.moveBack());
			REQUIRE(std::make_pair(fys::arena::HexagonSide::Hexagon::A, fys::arena::HexagonSide::Orientation::A_SE) == *hexagonC_NW);

		} // End section : All working back

		SECTION("All non working back") {
			REQUIRE_FALSE(hexagonA_S.moveBack());
			REQUIRE_FALSE(hexagonA_N.moveBack());
			REQUIRE_FALSE(hexagonA_SW.moveBack());
			REQUIRE_FALSE(hexagonA_NW.moveBack());

			REQUIRE_FALSE(hexagonB_N.moveBack());
			REQUIRE_FALSE(hexagonB_NW.moveBack());
			REQUIRE_FALSE(hexagonB_NE.moveBack());
			REQUIRE_FALSE(hexagonB_SE.moveBack());

			REQUIRE_FALSE(hexagonC_S.moveBack());
			REQUIRE_FALSE(hexagonC_SW.moveBack());
			REQUIRE_FALSE(hexagonC_SE.moveBack());
			REQUIRE_FALSE(hexagonC_NE.moveBack());
		} // End section : All non working back

	} // End section : test *

}
