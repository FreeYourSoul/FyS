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

#define FLATBUFFERS_DEBUG_VERIFICATION_FAILURE

#include <catch.hpp>
#include <zmq_addon.hpp>
#include <flatbuffers/flatbuffers.h>
#include <WSAction_generated.h>

TEST_CASE("FlatBuffer test default") {
//    flatbuffers::FlatBufferBuilder fbb;
//    fys_fb::MoveT move;
//    move.x = 1;
//    move.y = 2;
//    auto p = fys_fb::Move::Pack(fbb, &move);
//    fbb.Finish(p);
//
//    SECTION("Verifier") {
//        auto ok = flatbuffers::Verifier(fbb.GetBufferPointer(), fbb.GetSize());
//        REQUIRE(fys_fb::VerifyMoveBuffer(ok));
//    }
//    uint8_t *binary = fbb.GetBufferPointer();
//
//    SECTION("Binary to FlatBuffer") {
//        const fys_fb::Move *fromBinary = fys_fb::GetMove(binary);
//        REQUIRE(fromBinary->x() == move.x);
//        REQUIRE(fromBinary->y() == move.y);
//
//    } // End section : Binary to Flatbuffer
//
//
//    SECTION("ZMQ Message to FlatBuffer") {
//        zmq::message_t msg(binary, fbb.GetSize());
//        const fys_fb::Move *fromBinary = fys_fb::GetMove(msg.data());
//        REQUIRE(fromBinary->x() == move.x);
//        REQUIRE(fromBinary->y() == move.y);
//    }

}