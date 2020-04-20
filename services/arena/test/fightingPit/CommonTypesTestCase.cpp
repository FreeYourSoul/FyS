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

TEST_CASE("CommonTypesTestCase getActionNameFromKey test", "[service][arena]")
{
	REQUIRE("popo" == fys::arena::data::getActionNameFromKey("tete:tete:www:oo:popo.chai"));
	REQUIRE("popo" == fys::arena::data::getActionNameFromKey("tete:tete:www:oo:popo"));
	REQUIRE("popo" == fys::arena::data::getActionNameFromKey("popo.chai"));
	REQUIRE("popo" == fys::arena::data::getActionNameFromKey("popo"));

} // End TestCase : getActionNameFromKey test

TEST_CASE("CommonTypesTestCase getAlternateNameFromKey test", "[service][arena]")
{
	REQUIRE("alteration_popo" == fys::arena::data::getAlterationNameFromKey("tete:tete:www:oo:popo.chai"));
	REQUIRE("alteration_popo" == fys::arena::data::getAlterationNameFromKey("tete:tete:www:oo:popo"));
	REQUIRE("alteration_popo" == fys::arena::data::getAlterationNameFromKey("popo.chai"));
	REQUIRE("alteration_popo" == fys::arena::data::getAlterationNameFromKey("popo"));

} // End TestCase : getAlternateNameFromKey test

TEST_CASE("CommonTypesTestCase", "[service][arena]")
{
	std::vector<fys::arena::data::Alteration> alterations;

	fys::arena::data::mergeAlterations(alterations, {
			fys::arena::data::Alteration("burned", 1, 5, [](fys::arena::data::Status&, uint, uint) { return true; }),
			fys::arena::data::Alteration("burned", 1, 2, [](fys::arena::data::Status&, uint, uint) { return true; }),
			fys::arena::data::Alteration("burned", 1, 3, [](fys::arena::data::Status&, uint, uint) { return true; }),
			fys::arena::data::Alteration("frozen", 1, 1, [](fys::arena::data::Status&, uint, uint) { return true; }),
			fys::arena::data::Alteration("frozen", 1, 9, [](fys::arena::data::Status&, uint, uint) { return true; }),
			fys::arena::data::Alteration("frozen", 1, 8, [](fys::arena::data::Status&, uint, uint) { return true; }),
			fys::arena::data::Alteration("paralyzed", 1, 2, [](fys::arena::data::Status&, uint, uint) { return true; }),
			fys::arena::data::Alteration("paralyzed", 1, 42, [](fys::arena::data::Status&, uint, uint) { return true; }),
			fys::arena::data::Alteration("paralyzed", 1, 1, [](fys::arena::data::Status&, uint, uint) { return true; })
	}, true);

	REQUIRE(3 == alterations.size());
	REQUIRE(5 == alterations.at(0).getTurn());
	REQUIRE(9 == alterations.at(1).getTurn());
	REQUIRE(42 == alterations.at(2).getTurn());

} // End TestCase : WorkerService test