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
#include <fightingPit/data/CommonTypes.hh>

TEST_CASE("getActionNameFromKey test", "[service][arena]")
{
	REQUIRE("popo" == fys::arena::data::getActionNameFromKey("tete:tete:www:oo:popo.chai"));
	REQUIRE("popo" == fys::arena::data::getActionNameFromKey("tete:tete:www:oo:popo"));
	REQUIRE("popo" == fys::arena::data::getActionNameFromKey("popo.chai"));
	REQUIRE("popo" == fys::arena::data::getActionNameFromKey("popo"));

} // End TestCase : getActionNameFromKey test

TEST_CASE("getAlternateNameFromKey test", "[service][arena]")
{
	REQUIRE("alternate_popo" == fys::arena::data::getAlterationNameFromKey("tete:tete:www:oo:popo.chai"));
	REQUIRE("alternate_popo" == fys::arena::data::getAlterationNameFromKey("tete:tete:www:oo:popo"));
	REQUIRE("alternate_popo" == fys::arena::data::getAlterationNameFromKey("popo.chai"));
	REQUIRE("alternate_popo" == fys::arena::data::getAlterationNameFromKey("popo"));

} // End TestCase : getAlternateNameFromKey test

TEST_CASE("CommonTypes test", "[service][arena]")
{
	std::set<fys::arena::data::Alteration, fys::arena::data::Alteration::CompAlteration> alterations = {
			fys::arena::data::Alteration("burned", 1, 5, [](fys::arena::data::Status&, uint, uint) { return true; }),
			fys::arena::data::Alteration("burned", 1, 5, [](fys::arena::data::Status&, uint, uint) { return true; }),
			fys::arena::data::Alteration("burned", 1, 5, [](fys::arena::data::Status&, uint, uint) { return true; }),
			fys::arena::data::Alteration("frozen", 1, 5, [](fys::arena::data::Status&, uint, uint) { return true; }),
			fys::arena::data::Alteration("frozen", 1, 5, [](fys::arena::data::Status&, uint, uint) { return true; }),
			fys::arena::data::Alteration("frozen", 1, 5, [](fys::arena::data::Status&, uint, uint) { return true; }),
			fys::arena::data::Alteration("paralyzed", 1, 5, [](fys::arena::data::Status&, uint, uint) { return true; }),
			fys::arena::data::Alteration("paralyzed", 1, 5, [](fys::arena::data::Status&, uint, uint) { return true; }),
			fys::arena::data::Alteration("paralyzed", 1, 5, [](fys::arena::data::Status&, uint, uint) { return true; })
	};

	REQUIRE(3 == alterations.size());

} // End TestCase : WorkerService test