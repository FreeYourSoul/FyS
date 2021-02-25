// MIT License
//
// Copyright (c) 2021 Quentin Balland
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

#include <WSAction_generated.h>
#include <catch2/catch.hpp>
#include <flatbuffers/flatbuffers.h>
#include <zmq_addon.hpp>

TEST_CASE("WSActionFlatbufferTestCase WSAction Move", "[common][fb]") {
  flatbuffers::FlatBufferBuilder fbb;
  auto move = fys::fb::world::CreateMove(fbb, 42.42);

  auto p = fys::fb::world::CreateWSAction(fbb, fys::fb::world::Action::Action_Move, move.Union());
  fys::fb::world::FinishWSActionBuffer(fbb, p);

  SECTION("Verifier") {
    auto ok = flatbuffers::Verifier(fbb.GetBufferPointer(), fbb.GetSize());
    CHECK(fys::fb::world::VerifyWSActionBuffer(ok));
  }
  uint8_t* binary = fbb.GetBufferPointer();

  SECTION("Binary to FlatBuffer") {
    const fys::fb::world::WSAction* fromBinary = fys::fb::world::GetWSAction(binary);
    REQUIRE(fromBinary->action_as_Move()->direction() == 42.42);

  }// End section : Binary to Flatbuffer

  SECTION("ZMQ Message to FlatBuffer") {
    zmq::message_t msg(binary, fbb.GetSize());
    const fys::fb::world::WSAction* fromBinary = fys::fb::world::GetWSAction(msg.data());
    REQUIRE(fromBinary->action_as_Move()->direction() == 42.42);
  }
}

TEST_CASE("WSActionFlatbufferTestCase PnjInteract") {
  flatbuffers::FlatBufferBuilder fbb;
  auto pnjInterract = fys::fb::world::CreatePnjInteract(fbb, fbb.CreateString("idPnj"));
  auto token = fbb.CreateString("totoken");
  auto p = fys::fb::world::CreateWSAction(fbb, fys::fb::world::Action::Action_PnjInteract, pnjInterract.Union());
  fys::fb::world::FinishWSActionBuffer(fbb, p);

  SECTION("Verifier") {
    auto ok = flatbuffers::Verifier(fbb.GetBufferPointer(), fbb.GetSize());
    CHECK(fys::fb::world::VerifyWSActionBuffer(ok));
  }
  uint8_t* binary = fbb.GetBufferPointer();

  SECTION("Binary to FlatBuffer") {
    const fys::fb::world::WSAction* fromBinary = fys::fb::world::GetWSAction(binary);
    REQUIRE("idPnj" == std::string(fromBinary->action_as_PnjInteract()->idPnj()->c_str()));
  }// End section : Binary to Flatbuffer

  SECTION("ZMQ Message to FlatBuffer") {
    zmq::message_t msg(binary, fbb.GetSize());
    const fys::fb::world::WSAction* fromBinary = fys::fb::world::GetWSAction(msg.data());
    REQUIRE("idPnj" == std::string(fromBinary->action_as_PnjInteract()->idPnj()->c_str()));
  }
}