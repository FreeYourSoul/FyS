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
#include <ArenaServerAuth_generated.h>
#include <FightingPitEncounter_generated.h>

TEST_CASE("FlatBuffer ArenaCom FightingPitEncounter", "[common][fb]") {
    flatbuffers::FlatBufferBuilder fbb;
    auto fp = fys::fb::CreateFightingPitEncounter(
            fbb,
            fbb.CreateString("name"),
            fbb.CreateString("token_authentication"),
            fbb.CreateString("WS001"),
            0.0,
            0.0,
            fys::fb::Level::Level_EASY,
            4242,
            false,
            true);
    fys::fb::FinishFightingPitEncounterBuffer(fbb, fp);

    SECTION("Verifier") {
        auto ok = flatbuffers::Verifier(fbb.GetBufferPointer(), fbb.GetSize());
        CHECK(fys::fb::VerifyFightingPitEncounterBuffer(ok));
    }
    uint8_t *binary = fbb.GetBufferPointer();

    SECTION("Binary to FlatBuffer") {
        const fys::fb::FightingPitEncounter *fromBinary = fys::fb::GetFightingPitEncounter(binary);
        REQUIRE("name" == fromBinary->user_name()->str());
        REQUIRE("token_authentication" == fromBinary->token_auth()->str());
        REQUIRE(4242 == fromBinary->id_encounter());
        REQUIRE_FALSE(fromBinary->is_ambush());
        REQUIRE(fromBinary->new_encounter());

    } // End section : Binary to Flatbuffer

} // // End Test Case : FlatBuffer ArenaCom FightingPitEncounter

TEST_CASE("FlatBuffer ArenaCom ArenaServerAuth", "[common][fb]") {

    flatbuffers::FlatBufferBuilder fbb;
    auto asa = fys::fb::CreateArenaServerAuth(
            fbb,
            fbb.CreateString("name"),
            fbb.CreateString("token_authentication"),
            fbb.CreateString("localhost"),
            fbb.CreateString("tcp://localhost:4242"),
            42,
            fbb.CreateString("WS001"));
    fys::fb::FinishArenaServerAuthBuffer(fbb, asa);

    SECTION("Verifier") {
        auto ok = flatbuffers::Verifier(fbb.GetBufferPointer(), fbb.GetSize());
        CHECK(fys::fb::VerifyArenaServerAuthBuffer(ok));
    }
    uint8_t *binary = fbb.GetBufferPointer();

    SECTION("Binary to FlatBuffer") {
        const fys::fb::ArenaServerAuth *fromBinary = fys::fb::GetArenaServerAuth(binary);
        REQUIRE("name" == fromBinary->user_name()->str());
        REQUIRE("token_authentication" == fromBinary->token_auth()->str());
        REQUIRE("localhost" == fromBinary->ip()->str());
        REQUIRE("tcp://localhost:4242" == fromBinary->connection_str()->str());
        REQUIRE("WS001" == fromBinary->world_server_code()->str());
        REQUIRE(42 == fromBinary->id_fightingpit());

    } // End section : Binary to Flatbuffer

} // End Test Case : FlatBuffer ArenaCom ArenaServerAuth