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
#include <fightingPit/hexagon_side.hh>

TEST_CASE("HexagonSideTestCase", "[service][arena]") {
  SECTION("test ambush side") {

    SECTION("Not Ambush : A_N") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_N);
      REQUIRE_FALSE(hexagon.is_ambush_side());

    }// End section : Not Ambush : A_N

    SECTION("Not Ambush : A_NW") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_NW);
      REQUIRE_FALSE(hexagon.is_ambush_side());

    }// End section : Not Ambush : A_NW

    SECTION("Not Ambush : A_SW") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_SW);
      REQUIRE_FALSE(hexagon.is_ambush_side());

    }// End section : Not Ambush : A_SW

    SECTION("Not Ambush : A_S") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_S);
      REQUIRE_FALSE(hexagon.is_ambush_side());

    }// End section : Not Ambush : A_S

    SECTION("Not Ambush : B_N") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_N);
      REQUIRE_FALSE(hexagon.is_ambush_side());

    }// End section : Not Ambush : B_N

    SECTION("Not Ambush : B_NE") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_NE);
      REQUIRE_FALSE(hexagon.is_ambush_side());

    }// End section : Not Ambush : B_NE

    SECTION("Not Ambush : B_NW") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_NW);
      REQUIRE_FALSE(hexagon.is_ambush_side());

    }// End section : Not Ambush : B_NW

    SECTION("Not Ambush : B_SE") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SE);
      REQUIRE_FALSE(hexagon.is_ambush_side());

    }// End section : Not Ambush : B_SE

    SECTION("Not Ambush : C_S") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_S);
      REQUIRE_FALSE(hexagon.is_ambush_side());

    }// End section : Not Ambush : C_S

    SECTION("Not Ambush : C_SW") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_SW);
      REQUIRE_FALSE(hexagon.is_ambush_side());

    }// End section : Not Ambush : C_SW

    SECTION("Not Ambush : C_SE") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_SE);
      REQUIRE_FALSE(hexagon.is_ambush_side());

    }// End section : Not Ambush : C_SE

    SECTION("Not Ambush : C_NE") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_NE);
      REQUIRE_FALSE(hexagon.is_ambush_side());

    }// End section : Not Ambush : C_NE

    SECTION("Ambush : A_NE") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_NE);
      REQUIRE(hexagon.is_ambush_side());

    }// End section : Ambush : A_NE

    SECTION("Ambush : A_SE") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_SE);
      REQUIRE(hexagon.is_ambush_side());

    }// End section : Ambush : A_SE

    SECTION("Ambush : B_SW") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SW);
      REQUIRE(hexagon.is_ambush_side());

    }// End section : Ambush : B_SW

    SECTION("Ambush : B_S") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_S);
      REQUIRE(hexagon.is_ambush_side());

    }// End section : Ambush : B_S

    SECTION("Ambush : C_N") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_N);
      REQUIRE(hexagon.is_ambush_side());

    }// End section : Ambush : C_N

    SECTION("Ambush : C_NW") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_NW);
      REQUIRE(hexagon.is_ambush_side());

    }// End section : Ambush : C_NW

  }// End section : test ambush side

  SECTION("test Move") {

    SECTION("Force move") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_N);

      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_N) == *hexagon);
      REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_S));

      REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::B_S, true));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_S) == *hexagon);

      REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_S));
      REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_S, true));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_S) == *hexagon);

    }// End section : Force move

    SECTION("Hexagon A : A_N") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_N);

      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_N) == *hexagon);

      SECTION("Not possible") {
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_SW));
      }// End section : Not possible

      SECTION("to A_NE") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::A_NE));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_NE) == *hexagon);
      }// End section : toNE

      SECTION("to A_NW") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::A_NW));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_NW) == *hexagon);
      }// End section : toNW

    }// End section : Hexagon A

    SECTION("Hexagon A : A_NE") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_NE);

      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_NE) == *hexagon);

      SECTION("Not possible") {
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_SW));

      }// End section : Not possible

      SECTION("to A_N") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::A_N));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_N) == *hexagon);
      }// End section : toN

      SECTION("to A_SE") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::A_SE));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_SE) == *hexagon);
      }// End section : toSE

      SECTION("to B_NW") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::B_NW));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_NW) == *hexagon);
      }// End section : to B_NW

      SECTION("to B_S") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::B_S));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_S) == *hexagon);
      }// End section : to B_S

      SECTION("to B_SW") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::B_SW));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SW) == *hexagon);
      }// End section : to B_SW

      SECTION("to C_N") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_N));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_N) == *hexagon);
      }// End section : to C_N

      SECTION("to C_NW") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_NW));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_NW) == *hexagon);
      }// End section : to C_NW

    }// End section : Hexagon A

    SECTION("Hexagon A : A_NW") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_NW);

      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_NW) == *hexagon);

      SECTION("Not Possible") {
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_SW));

      }// End section : Not Possible

      SECTION("to A_N") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::A_N));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_N) == *hexagon);

      }// End section : to A_N

      SECTION("to A_SW") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::A_SW));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_SW) == *hexagon);

      }// End section : to A_SW

    }// End section : Hexagon A

    SECTION("Hexagon A : A_S") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_S);

      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_S) == *hexagon);

      SECTION("Not Possible") {
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));

      }// End section : Not Possible

      SECTION("to A_SW") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::A_SW));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_SW) == *hexagon);

      }// End section : to A_SW

      SECTION("to A_SE") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::A_SE));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_SE) == *hexagon);

      }// End section : to A_SE

      SECTION("to C_NW") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_NW));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_NW) == *hexagon);

      }// End section : to C_NW

      SECTION("to C_SW") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_SW));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_SW) == *hexagon);

      }// End section : C_SW

    }// End section : Hexagon A : A_S

    SECTION("Hexagon A : A_SE") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_SE);

      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_SE) == *hexagon);

      SECTION("Not Possible") {
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));

      }// End section : Not Possible

      SECTION("to A_NE") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::A_NE));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_NE) == *hexagon);

      }// End section : to A_NE

      SECTION("to A_S") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::A_S));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_S) == *hexagon);

      }// End section : to A_S

      SECTION("to B_S") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::B_S));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_S) == *hexagon);

      }// End section : to B_S

      SECTION("to B_SW") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::B_SW));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SW) == *hexagon);

      }// End section : to B_SW

      SECTION("to C_N") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_N));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_N) == *hexagon);

      }// End section : to C_N

      SECTION("to C_NW") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_NW));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_NW) == *hexagon);

      }// End section : to C_NW

      SECTION("to C_SW") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_SW));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_SW) == *hexagon);

      }// End section : to C_SW

    }// End section : Hexagon A : A_SE

    SECTION("Hexagon A : A_SW") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_SW);

      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_SW) == *hexagon);

      SECTION("Not Possible") {
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_SW));

      }// End section : Not Possible

      SECTION("to A_NW") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::A_NW));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_NW) == *hexagon);

      }// End section : to A_NW

      SECTION("to A_S") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::A_S));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_S) == *hexagon);

      }// End section : to A_S

    }// End section : Hexagon A : A_SW

    SECTION("Impossible Position") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SW);
      REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::NONE));
    }// End section

    SECTION("Hexagon B : B_N") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_N);

      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_N) == *hexagon);

      SECTION("Not Possible") {
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_SW));

      }// End section : Not Possible

      SECTION("to B_NE") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::B_NE));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_NE) == *hexagon);

      }// End section : to B_NE

      SECTION("to B_NW") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::B_NW));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_NW) == *hexagon);

      }// End section : to B_NW

    }// End section : Hexagon B : B_N

    SECTION("Hexagon B : B_NE") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_NE);

      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_NE) == *hexagon);

      SECTION("Not Possible") {
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_SW));

      }// End section : Not Possible

      SECTION("to B_N") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::B_N));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_N) == *hexagon);

      }// End section : to B_N

      SECTION("to B_SE") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::B_SE));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SE) == *hexagon);

      }// End section : to B_SE

    }// End section : Hexagon B : B_NE

    SECTION("Hexagon B : B_NW") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_NW);

      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_NW) == *hexagon);

      SECTION("Not Possible") {
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_SW));

      }// End section : Not Possible

      SECTION("to B_N") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::B_N));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_N) == *hexagon);

      }// End section : to B_N

      SECTION("to B_SW") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::B_SW));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SW) == *hexagon);

      }// End section : to B_SW

      SECTION("to A_N") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::A_N));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_N) == *hexagon);

      }// End section : to A_N

      SECTION("to A_NE") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::A_NE));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_NE) == *hexagon);

      }// End section : to A_NE

    }// End section : Hexagon B : B_NW

    SECTION("Hexagon B : B_S") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_S);

      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_S) == *hexagon);

      SECTION("Not Possible") {
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_SW));

      }// End section : Not Possible

      SECTION("to B_SE") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::B_SE));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SE) == *hexagon);

      }// End section : to B_SE

      SECTION("to B_SW") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::B_SW));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SW) == *hexagon);

      }// End section : to B_SW

      SECTION("to C_N") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_N));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_N) == *hexagon);

      }// End section : to C_N

      SECTION("to C_NE") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_NE) == *hexagon);

      }// End section : to C_NE

      SECTION("to C_NW") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_NW));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_NW) == *hexagon);

      }// End section : to C_NW

      SECTION("to A_SE") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::A_SE));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_SE) == *hexagon);

      }// End section : to A_SE

      SECTION("to A_NE") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::A_NE));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_NE) == *hexagon);

      }// End section : to A_NE

    }// End section : Hexagon B : B_S

    SECTION("Hexagon B : B_SW") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SW);

      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SW) == *hexagon);

      SECTION("Not Possible") {
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_SW));

      }// End section : Not Possible

      SECTION("to B_S") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::B_S));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_S) == *hexagon);

      }// End section : to B_S

      SECTION("to B_NW") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::B_NW));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_NW) == *hexagon);

      }// End section : to B_NW

      SECTION("to A_N") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::A_N));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_N) == *hexagon);

      }// End section : to A_N

      SECTION("to A_NE") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::A_NE));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_NE) == *hexagon);

      }// End section : to A_NE

      SECTION("to A_SE") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::A_SE));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_SE) == *hexagon);

      }// End section : to A_SE

      SECTION("to C_N") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_N));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_N) == *hexagon);

      }// End section : to C_N

      SECTION("to C_NW") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_NW));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_NW) == *hexagon);

      }// End section : to C_NW

    }// End section : Hexagon B : B_SW

    SECTION("Hexagon B : B_SE") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SE);

      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SE) == *hexagon);

      SECTION("Not Possible") {
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_SW));

      }// End section : Not Possible

      SECTION("to B_S") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::B_S));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_S) == *hexagon);

      }// End section : to B_S

      SECTION("to B_NE") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::B_NE));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_NE) == *hexagon);

      }// End section : to B_NE

      SECTION("to C_N") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_N));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_N) == *hexagon);

      }// End section : to C_N

      SECTION("to C_NE") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_NE) == *hexagon);

      }// End section : to C_NE

    }// End section : Hexagon B : B_SW

    SECTION("Hexagon C : C_N") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_N);

      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_N) == *hexagon);

      SECTION("Not Possible") {
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_SW));

      }// End section : Not Possible

      SECTION("to C_NE") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_NE) == *hexagon);

      }// End section : to C_NE

      SECTION("to C_NW") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_NW));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_NW) == *hexagon);

      }// End section : to C_NW

      SECTION("to B_S") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::B_S));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_S) == *hexagon);

      }// End section : to B_S

      SECTION("to B_SE") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::B_SE));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SE) == *hexagon);

      }// End section : to B_SE

      SECTION("to B_SW") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::B_SW));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SW) == *hexagon);

      }// End section : to B_SW

      SECTION("to A_NE") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::A_NE));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_NE) == *hexagon);

      }// End section : to A_NE

      SECTION("to A_SE") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::A_SE));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_SE) == *hexagon);

      }// End section : to A_SE

    }// End section : Hexagon C : C_N

    SECTION("Hexagon C : C_NE") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_NE);

      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_NE) == *hexagon);

      SECTION("Not Possible") {
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_SW));

      }// End section : Not Possible

      SECTION("to C_N") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_N));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_N) == *hexagon);

      }// End section : to C_N

      SECTION("to C_SE") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_SE));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_SE) == *hexagon);

      }// End section : to C_SE

      SECTION("to B_S") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::B_S));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_S) == *hexagon);

      }// End section : to B_S

      SECTION("to B_SE") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::B_SE));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SE) == *hexagon);

      }// End section : to B_SE

    }// End section : Hexagon C : C_NE

    SECTION("Hexagon C : C_NW") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_NW);

      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_NW) == *hexagon);

      SECTION("Not Possible") {
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_S));

      }// End section : Not Possible

      SECTION("to C_N") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_N));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_N) == *hexagon);

      }// End section : to C_N

      SECTION("to C_SW") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_SW));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_SW) == *hexagon);

      }// End section : to to C_SW

      SECTION("to A_S") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::A_S));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_S) == *hexagon);

      }// End section : to A_S

      SECTION("to A_SE") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::A_SE));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_SE) == *hexagon);

      }// End section : to A_SE

      SECTION("to A_NE") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::A_NE));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_NE) == *hexagon);

      }// End section : to A_NE

      SECTION("to B_S") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::B_S));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_S) == *hexagon);

      }// End section : to C_S

      SECTION("to B_SW") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::B_SW));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SW) == *hexagon);

      }// End section : to C_SW

    }// End section : Hexagon C : C_NW

    SECTION("Hexagon C : C_S") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_S);

      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_S) == *hexagon);

      SECTION("Not Possible") {
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_S));

      }// End section : Not Possible

      SECTION("to C_SW") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_SW));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_SW) == *hexagon);

      }// End section : to C_SW

      SECTION("to C_SE") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_SE));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_SE) == *hexagon);

      }// End section : to C_SE

    }// End section : Hexagon C : C_S

    SECTION("Hexagon C : C_SE") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_SE);

      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_SE) == *hexagon);

      SECTION("Not Possible") {
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_SW));

      }// End section : Not Possible

      SECTION("to C_S") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_S));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_S) == *hexagon);

      }// End section : to C_S

      SECTION("to C_NE") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_NE) == *hexagon);

      }// End section : to C_NE

    }// End section : Hexagon C : C_SE

    SECTION("Hexagon C : C_SW") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_SW);

      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_SW) == *hexagon);

      SECTION("Not Possible") {
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_N));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_SE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::A_NE));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::C_SW));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_S));
        REQUIRE_FALSE(hexagon.move(fys::arena::hexagon_side::orientation::B_SW));

      }// End section : Not Possible

      SECTION("to C_S") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_S));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_S) == *hexagon);

      }// End section : to C_S

      SECTION("to C_NW") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::C_NW));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_NW) == *hexagon);

      }// End section : to C_NW

      SECTION("to A_S") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::A_S));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_S) == *hexagon);

      }// End section : to A_S

      SECTION("to A_SE") {
        REQUIRE(hexagon.move(fys::arena::hexagon_side::orientation::A_SE));
        REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_SE) == *hexagon);

      }// End section : to A_SE

    }// End section : Hexagon C : C_SW

  }// End section : test Move

  SECTION("test MoveLeft") {

    SECTION("Hexagone A full circle by left") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_SE);

      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_SE) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_left()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_NE) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_left()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_N) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_left()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_NW) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_left()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_SW) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_left()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_S) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_left()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_SE) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_left()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_NE) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_left()));
    }// End section : Hexagone A full circle by left

    SECTION("Hexagone B full circle by left") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SE);

      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SE) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_left()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_NE) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_left()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_N) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_left()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_NW) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_left()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SW) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_left()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_S) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_left()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SE) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_left()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_NE) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_left()));
    }// End section : Hexagone B full circle by left

    SECTION("Hexagone C full circle by left") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_SE);

      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_SE) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_left()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_NE) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_left()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_N) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_left()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_NW) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_left()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_SW) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_left()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_S) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_left()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_SE) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_left()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_NE) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_left()));
    }// End section : Hexagone C full circle by left

  }// End section : MoveLeft

  SECTION("test MoveRight") {

    SECTION("Hexagone A full circle by right") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_SE);

      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_SE) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_right()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_S) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_right()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_SW) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_right()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_NW) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_right()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_N) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_right()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_NE) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_right()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_SE) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_right()));

    }// End section : Hexagone A full circle by right

    SECTION("Hexagone B full circle by right") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SE);

      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SE) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_right()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_S) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_right()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SW) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_right()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_NW) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_right()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_N) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_right()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_NE) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_right()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SE) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_right()));

    }// End section : Hexagone B full circle by right

    SECTION("Hexagone C full circle by right") {
      fys::arena::hexagon_side hexagon(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_SE);

      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_SE) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_right()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_S) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_right()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_SW) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_right()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_NW) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_right()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_N) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_right()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_NE) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_right()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_SE) == *hexagon);
      REQUIRE(hexagon.move(hexagon.find_right()));
    }// End section : Hexagone C full circle by right

  }// End section : MoveRight

  SECTION("test moveBack") {
    fys::arena::hexagon_side hexagonA_S(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_S);
    fys::arena::hexagon_side hexagonA_SE(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_SE);
    fys::arena::hexagon_side hexagonA_SW(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_SW);
    fys::arena::hexagon_side hexagonA_N(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_N);
    fys::arena::hexagon_side hexagonA_NE(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_NE);
    fys::arena::hexagon_side hexagonA_NW(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_NW);

    fys::arena::hexagon_side hexagonB_S(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_S);
    fys::arena::hexagon_side hexagonB_SE(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SE);
    fys::arena::hexagon_side hexagonB_SW(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SW);
    fys::arena::hexagon_side hexagonB_N(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_N);
    fys::arena::hexagon_side hexagonB_NE(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_NE);
    fys::arena::hexagon_side hexagonB_NW(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_NW);

    fys::arena::hexagon_side hexagonC_S(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_S);
    fys::arena::hexagon_side hexagonC_SE(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_SE);
    fys::arena::hexagon_side hexagonC_SW(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_SW);
    fys::arena::hexagon_side hexagonC_N(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_N);
    fys::arena::hexagon_side hexagonC_NE(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_NE);
    fys::arena::hexagon_side hexagonC_NW(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_NW);

    SECTION("All working back") {
      REQUIRE(hexagonA_SE.move(hexagonA_SE.find_back()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_NW) == *hexagonA_SE);

      REQUIRE(hexagonA_NE.move(hexagonA_NE.find_back()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_SW) == *hexagonA_NE);

      REQUIRE(hexagonB_SW.move(hexagonB_SW.find_back()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_NE) == *hexagonB_SW);

      REQUIRE(hexagonB_S.move(hexagonB_S.find_back()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::C, fys::arena::hexagon_side::orientation::C_N) == *hexagonB_S);

      REQUIRE(hexagonC_N.move(hexagonC_N.find_back()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::B, fys::arena::hexagon_side::orientation::B_S) == *hexagonC_N);

      REQUIRE(hexagonC_NW.move(hexagonC_NW.find_back()));
      REQUIRE(std::pair(fys::arena::hexagon_side::hexagon::A, fys::arena::hexagon_side::orientation::A_SE) == *hexagonC_NW);

    }// End section : All working back

    SECTION("All non working back") {
      REQUIRE_FALSE(hexagonA_S.move(hexagonA_S.find_back()));
      REQUIRE_FALSE(hexagonA_N.move(hexagonA_N.find_back()));
      REQUIRE_FALSE(hexagonA_SW.move(hexagonA_SW.find_back()));
      REQUIRE_FALSE(hexagonA_NW.move(hexagonA_NW.find_back()));

      REQUIRE_FALSE(hexagonB_N.move(hexagonB_N.find_back()));
      REQUIRE_FALSE(hexagonB_NW.move(hexagonB_NW.find_back()));
      REQUIRE_FALSE(hexagonB_NE.move(hexagonB_NE.find_back()));
      REQUIRE_FALSE(hexagonB_SE.move(hexagonB_SE.find_back()));

      REQUIRE_FALSE(hexagonC_S.move(hexagonC_S.find_back()));
      REQUIRE_FALSE(hexagonC_SW.move(hexagonC_SW.find_back()));
      REQUIRE_FALSE(hexagonC_SE.move(hexagonC_SE.find_back()));
      REQUIRE_FALSE(hexagonC_NE.move(hexagonC_NE.find_back()));
    }// End section : All non working back

  }// End section : test *
}
