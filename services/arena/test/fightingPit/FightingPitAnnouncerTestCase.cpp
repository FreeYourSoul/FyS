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
#include <FSeamMockData.hpp>
#include <ArenaServerContext.hh>
#include <fightingPit/FightingPitAnnouncer.hh>
#include <fightingPit/contender/FightingContender.hh>
#include <fightingPit/contender/ContenderScripting.hh>
#include <RandomGenerator.hh>
#include <CmlKey.hh>
#include <Cml.hh>


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
    fys::arena::ConnectionHandler handler {};
    auto fseamConnectionHandlerMock = FSeam::get(&handler);
    auto cml = CmlBase(getLocalPathStorage());
    fys::arena::EncounterContext ctx;
    ctx._rangeEncounterPerZone["WS00"] = {
            std::make_pair(1, 4), // ez
            std::make_pair(2, 4), // medium
            std::make_pair(3, 5)  // hard
    };
    ctx._contendersPerZone["WS00"] = {
            fys::arena::EncounterContext::EncounterDesc{
                    "arena:contenders:Sampy.chai", 3,
                    {60, 60, 60}
            },
            fys::arena::EncounterContext::EncounterDesc {
                    "arena:contenders:Slime.chai", 3,
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

        SECTION("test generate contender Easy") { // seed 2 80 10 28 8
            fys::arena::FightingPitAnnouncer fpa(cml);
            fpa.setDifficulty(fys::arena::FightingPit::EASY);
            fpa.setEncounterType(fys::arena::FightingPitAnnouncer::EncounterType::RANDOM);
            auto fightingPit = fpa.buildFightingPit(ctx, handler, "WS00");

            REQUIRE(2 == fightingPit->getPitContenders().getNumberContender());

            REQUIRE("Slime" == fightingPit->getPitContenders().getFightingContender(0)->getContenderScripting()->getContenderName());
            REQUIRE(0 == fightingPit->getPitContenders().getFightingContender(0)->getContenderScripting()->getContenderId());
            REQUIRE(10 == fightingPit->getPitContenders().getFightingContender(0)->getContenderScripting()->getLevel());

            REQUIRE("Sampy" == fightingPit->getPitContenders().getFightingContender(1)->getContenderScripting()->getContenderName());
            REQUIRE(1 == fightingPit->getPitContenders().getFightingContender(1)->getContenderScripting()->getContenderId());
            REQUIRE(8 == fightingPit->getPitContenders().getFightingContender(1)->getContenderScripting()->getLevel());

        } // End section : test generate contender Easy

        SECTION("test generate contender Medium") { // seed 3 80 10 18 8 78 6
            fys::arena::FightingPitAnnouncer fpa(cml);
            fpa.setDifficulty(fys::arena::FightingPit::MEDIUM);
            fpa.setEncounterType(fys::arena::FightingPitAnnouncer::EncounterType::RANDOM);
            auto fightingPit = fpa.buildFightingPit(ctx, handler, "WS00");

            REQUIRE(3 == fightingPit->getPitContenders().getNumberContender());

            REQUIRE("Slime" == fightingPit->getPitContenders().getFightingContender(0)->getContenderScripting()->getContenderName());
            REQUIRE(0 == fightingPit->getPitContenders().getFightingContender(0)->getContenderScripting()->getContenderId());
            REQUIRE(10 == fightingPit->getPitContenders().getFightingContender(0)->getContenderScripting()->getLevel());

            REQUIRE("Sampy" == fightingPit->getPitContenders().getFightingContender(1)->getContenderScripting()->getContenderName());
            REQUIRE(1 == fightingPit->getPitContenders().getFightingContender(1)->getContenderScripting()->getContenderId());
            REQUIRE(8 == fightingPit->getPitContenders().getFightingContender(1)->getContenderScripting()->getLevel());

            REQUIRE("Slime" == fightingPit->getPitContenders().getFightingContender(2)->getContenderScripting()->getContenderName());
            REQUIRE(2 == fightingPit->getPitContenders().getFightingContender(2)->getContenderScripting()->getContenderId());
            REQUIRE(6 == fightingPit->getPitContenders().getFightingContender(2)->getContenderScripting()->getLevel());

        } // End section : test generate contender Medium

        SECTION("test generate contender Hard") { // seed 4 80 10 18 8 78 6 60 2
            fys::arena::FightingPitAnnouncer fpa(cml);
            fpa.setDifficulty(fys::arena::FightingPit::HARD);
            fpa.setEncounterType(fys::arena::FightingPitAnnouncer::EncounterType::RANDOM);
            auto fightingPit = fpa.buildFightingPit(ctx, handler, "WS00");

            REQUIRE(4 == fightingPit->getPitContenders().getNumberContender());

            REQUIRE("Slime" == fightingPit->getPitContenders().getFightingContender(0)->getContenderScripting()->getContenderName());
            REQUIRE(0 == fightingPit->getPitContenders().getFightingContender(0)->getContenderScripting()->getContenderId());
            REQUIRE(10 == fightingPit->getPitContenders().getFightingContender(0)->getContenderScripting()->getLevel());

            REQUIRE("Sampy" == fightingPit->getPitContenders().getFightingContender(1)->getContenderScripting()->getContenderName());
            REQUIRE(1 == fightingPit->getPitContenders().getFightingContender(1)->getContenderScripting()->getContenderId());
            REQUIRE(8 == fightingPit->getPitContenders().getFightingContender(1)->getContenderScripting()->getLevel());

            REQUIRE("Slime" == fightingPit->getPitContenders().getFightingContender(2)->getContenderScripting()->getContenderName());
            REQUIRE(2 == fightingPit->getPitContenders().getFightingContender(2)->getContenderScripting()->getContenderId());
            REQUIRE(6 == fightingPit->getPitContenders().getFightingContender(2)->getContenderScripting()->getLevel());

            REQUIRE("Sampy" == fightingPit->getPitContenders().getFightingContender(3)->getContenderScripting()->getContenderName());
            REQUIRE(3 == fightingPit->getPitContenders().getFightingContender(3)->getContenderScripting()->getContenderId());
            REQUIRE(2 == fightingPit->getPitContenders().getFightingContender(3)->getContenderScripting()->getLevel());

        } // End section : test generate contender Hard

    } // End section : RNG test 42

    SECTION("test seed 1337") {
        std::shared_ptr<std::mt19937> mt = std::make_shared<std::mt19937>(1337);
        fseamMock->dupeReturn<FSeam::RandomGenerator::get>(mt);

        SECTION("test seed Easy") {
            REQUIRE(2 == fys::util::RandomGenerator::generateInRange(1, 4));
            // encounter 1
            REQUIRE(56 == fys::util::RandomGenerator::generateInRange(0, 100));
            REQUIRE(2 == fys::util::RandomGenerator::generateInRange(1, 10));
            // encounter 2
            REQUIRE(21 == fys::util::RandomGenerator::generateInRange(0, 100));
            REQUIRE(3 == fys::util::RandomGenerator::generateInRange(1, 10));
        } // End section : test seed Easy

        SECTION("test generate contender Easy") { // seed 2 56 2 21 3
            fys::arena::FightingPitAnnouncer fpa(cml);
            fpa.setDifficulty(fys::arena::FightingPit::EASY);
            fpa.setEncounterType(fys::arena::FightingPitAnnouncer::EncounterType::RANDOM);
            auto fightingPit = fpa.buildFightingPit(ctx, handler, "WS00");

            REQUIRE(2 == fightingPit->getPitContenders().getNumberContender());

            REQUIRE("Sampy" == fightingPit->getPitContenders().getFightingContender(0)->getContenderScripting()->getContenderName());
            REQUIRE(0 == fightingPit->getPitContenders().getFightingContender(0)->getContenderScripting()->getContenderId());
            REQUIRE(2 == fightingPit->getPitContenders().getFightingContender(0)->getContenderScripting()->getLevel());

            REQUIRE("Sampy" == fightingPit->getPitContenders().getFightingContender(1)->getContenderScripting()->getContenderName());
            REQUIRE(1 == fightingPit->getPitContenders().getFightingContender(1)->getContenderScripting()->getContenderId());
            REQUIRE(3 == fightingPit->getPitContenders().getFightingContender(1)->getContenderScripting()->getLevel());

        } // End section : test generate contender Easy

    } // End section : RNG test 1337

    FSeam::MockVerifier::cleanUp();
} // End TestCase : FightingPitAnnouncer test



#pragma clang diagnostic pop