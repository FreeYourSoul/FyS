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
#include <engine/PlayersData.hh>

#include <WSResponse_generated.h>

#include <FlatbufferGenerator.hh>

using namespace fys::ws;

namespace {
template<typename T>
[[nodiscard]] bool
verifyBuffer(const void* fbBuffer, uint size)
{
	auto v = flatbuffers::Verifier(static_cast<const uint8_t*>(fbBuffer), size);
	return v.VerifyBuffer<T>();
}
}

TEST_CASE("ws::FlatbufferGeneratorTestCase", "[service][world][util]")
{
	fys::ws::FlatbufferGenerator fg;

	SECTION("generateMoveNotification") {

		auto[data, size] = fg.generateMoveNotification("Naming", CharacterInfo{Pos{11, 22}, 1337.0, 42.42});

		REQUIRE(nullptr != data);
		REQUIRE(0 < size);
		REQUIRE(verifyBuffer<fys::fb::world::ResponseFrame>(data, size));

		auto* binary = fys::fb::world::GetResponseFrame(data);

		REQUIRE(fys::fb::world::Response_MoveNotification == binary->content_type());

		auto* moveNotification = binary->content_as_MoveNotification();

		REQUIRE("Naming" == moveNotification->id()->string_view());
		REQUIRE(11 == moveNotification->x());
		REQUIRE(22 == moveNotification->y());
		REQUIRE(1337.0 == moveNotification->velocity());
		REQUIRE(42.42 == moveNotification->dir());

	} // End section : generateMoveNotification
	
} // End TestCase : ws::FlatbufferGeneratorTestCase

