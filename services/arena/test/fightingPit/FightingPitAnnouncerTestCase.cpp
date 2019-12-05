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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-msc32-c"

#include <catch2/catch.hpp>
#include <fightingPit/FightingPitAnnouncer.hh>
#include <RandomGenerator.hh>
#include <CmlKey.hh>
#include <Cml.hh>
#include <ArenaServerContext.hh>
#include <FSeamMockData.hpp>

class CmlBase : public fys::cache::Cml {
public:
    explicit CmlBase(std::string v) : fys::cache::Cml(std::filesystem::path(std::move(v))) {}

    void createFileInLocalStorage(const fys::cache::CmlKey &cmlKey) override {
    }
};

namespace {
    std::string getLocalPathStorage() {
        std::string file_path = __FILE__;
        std::string dir_path = file_path.substr(0, file_path.rfind('\\'));
        if (dir_path.size() == file_path.size())
            dir_path = file_path.substr(0, file_path.rfind('/'));
        return dir_path + "/../../scriptTests/scripts_lnk";
    }
}

TEST_CASE("FightingPitAnnouncer test", "[service][arena]") {
    auto fseamMock = FSeam::getDefault<fys::util::RandomGenerator>();
    auto cml = CmlBase(getLocalPathStorage());
    fys::arena::EncounterContext ctx;
    ctx._rangeEncounterPerZone["WS00"] = {
            std::make_pair(1, 4), // ez
            std::make_pair(2, 4), // medium
            std::make_pair(3, 5)  // hard
    };
    ctx._contendersPerZone["WS00"] = {
            fys::arena::EncounterContext::EncounterDesc{
                    "arena::contenders::Sampy.chai", 3,
                    {60, 60, 60}
            },
            fys::arena::EncounterContext::EncounterDesc {
                    "arena::contenders::Sampy.chai", 3,
                    {40, 40, 40}
            }
    };

    SECTION("test seed 42") {
        std::shared_ptr<std::mt19937> mt = std::make_shared<std::mt19937>(42);
        fseamMock->dupeReturn<FSeam::RandomGenerator::get>(mt);

        SECTION("test seed ez") {
            REQUIRE(2 == fys::util::RandomGenerator::generateInRange(1, 4));
            // encounter 1
            REQUIRE(80 == fys::util::RandomGenerator::generateInRange(0, 100));
            REQUIRE(10 == fys::util::RandomGenerator::generateInRange(1, 10));
            // encounter 2
            REQUIRE(18 == fys::util::RandomGenerator::generateInRange(0, 100));
            REQUIRE(8 == fys::util::RandomGenerator::generateInRange(1, 10));

        } // End section : test seed ez
        SECTION("test seed medium") {
            REQUIRE(3 == fys::util::RandomGenerator::generateInRange(2, 4));
            // encounter 1
            REQUIRE(80 == fys::util::RandomGenerator::generateInRange(0, 100));
            REQUIRE(10 == fys::util::RandomGenerator::generateInRange(1, 10));
            // encounter 2
            REQUIRE(18 == fys::util::RandomGenerator::generateInRange(0, 100));
            REQUIRE(8 == fys::util::RandomGenerator::generateInRange(1, 10));
            // encounter 3
            REQUIRE(78 == fys::util::RandomGenerator::generateInRange(0, 100));
            REQUIRE(6 == fys::util::RandomGenerator::generateInRange(1, 10));
        } // End section : test seed medium

        SECTION("test seed hard") {
            REQUIRE(4 == fys::util::RandomGenerator::generateInRange(3, 5));
            // encounter 1
            REQUIRE(80 == fys::util::RandomGenerator::generateInRange(0, 100));
            REQUIRE(10 == fys::util::RandomGenerator::generateInRange(1, 10));
            // encounter 2
            REQUIRE(18 == fys::util::RandomGenerator::generateInRange(0, 100));
            REQUIRE(8 == fys::util::RandomGenerator::generateInRange(1, 10));
            // encounter 3
            REQUIRE(78 == fys::util::RandomGenerator::generateInRange(0, 100));
            REQUIRE(6 == fys::util::RandomGenerator::generateInRange(1, 10));
            // encounter 4
            REQUIRE(60 == fys::util::RandomGenerator::generateInRange(0, 100));
            REQUIRE(2 == fys::util::RandomGenerator::generateInRange(1, 10));
        } // End section : test seed hard

        SECTION("test generate contender") {
            fys::arena::FightingPitAnnouncer fpa(cml);
            fpa.setDifficulty(fys::arena::FightingPit::EASY);

        } // End section : test generate contender

    } // End section : RNG test 42

    SECTION("test seed 1337") {
        std::mt19937 mt(1337);

        SECTION("test seed") {

        } // End section : test seed

        SECTION("test generate contender") {
            fys::arena::FightingPitAnnouncer fpa(cml);

        } // End section : test generate contender

    } // End section : RNG test 1337

} // End TestCase : FightingPitAnnouncer test



#pragma clang diagnostic pop