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
#include <zmq_addon.hpp>
#include <flatbuffers/flatbuffers.h>
#include <WSAction_generated.h>

TEST_CASE("FlatBuffer WSAction Move") {
    flatbuffers::FlatBufferBuilder fbb;
    auto move = fys::fb::CreateMove(fbb, 42.42);
    auto token = fbb.CreateString("totoken");
    auto p = fys::fb::CreateWSAction(fbb, fys::fb::Action::Action_Move, move.Union(), token);
    fys::fb::FinishWSActionBuffer(fbb, p);

    SECTION("Verifier") {
        auto ok = flatbuffers::Verifier(fbb.GetBufferPointer(), fbb.GetSize());
        CHECK(fys::fb::VerifyWSActionBuffer(ok));
    }
    uint8_t *binary = fbb.GetBufferPointer();

    SECTION("Binary to FlatBuffer") {
        const fys::fb::WSAction *fromBinary = fys::fb::GetWSAction(binary);
        REQUIRE(fromBinary->action_as_Move()->direction() == 42.42);
        REQUIRE("totoken" == std::string(fromBinary->token_client()->c_str()));

    } // End section : Binary to Flatbuffer


    SECTION("ZMQ Message to FlatBuffer") {
        zmq::message_t msg(binary, fbb.GetSize());
        const fys::fb::WSAction *fromBinary = fys::fb::GetWSAction(msg.data());
        REQUIRE(fromBinary->action_as_Move()->direction() == 42.42);
        REQUIRE("totoken" == std::string(fromBinary->token_client()->c_str()));
    }

}

TEST_CASE("FlatBuffer WSAction PnjInteract") {
    flatbuffers::FlatBufferBuilder fbb;
    auto pnjInterract = fys::fb::CreatePnjInteract(fbb, fbb.CreateString("idPnj"));
    auto token = fbb.CreateString("totoken");
    auto p = fys::fb::CreateWSAction(fbb, fys::fb::Action::Action_PnjInteract, pnjInterract.Union(), token);
    fys::fb::FinishWSActionBuffer(fbb, p);

    SECTION("Verifier") {
        auto ok = flatbuffers::Verifier(fbb.GetBufferPointer(), fbb.GetSize());
        CHECK(fys::fb::VerifyWSActionBuffer(ok));
    }
    uint8_t *binary = fbb.GetBufferPointer();

    SECTION("Binary to FlatBuffer") {
        const fys::fb::WSAction *fromBinary = fys::fb::GetWSAction(binary);
        REQUIRE("idPnj" == std::string(fromBinary->action_as_PnjInteract()->idPnj()->c_str()));
        REQUIRE("totoken" == std::string(fromBinary->token_client()->c_str()));

    } // End section : Binary to Flatbuffer


    SECTION("ZMQ Message to FlatBuffer") {
        zmq::message_t msg(binary, fbb.GetSize());
        const fys::fb::WSAction *fromBinary = fys::fb::GetWSAction(msg.data());
        REQUIRE("idPnj" == std::string(fromBinary->action_as_PnjInteract()->idPnj()->c_str()));
        REQUIRE("totoken" == std::string(fromBinary->token_client()->c_str()));
    }

}