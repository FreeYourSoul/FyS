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
#include <chaiscript/chaiscript.hpp>
#include <ArenaServerContext.hh>
#include <ChaiRegister.hh>
#include <fightingPit/contender/ContenderScripting.hh>
#include <fightingPit/contender/FightingContender.hh>
#include <fightingPit/contender/PitContenders.hh>
#include <fightingPit/team/AllyPartyTeams.hh>
#include <fightingPit/team/PartyTeam.hh>
#include <fightingPit/team/TeamMember.hh>

#include <FSeamMockData.hpp>

#include <ChaiRegister.hh>
#include <CmlCopy.hh>

namespace {
    std::string getPathSampyChaiScript() {
        std::string file_path = __FILE__;
        std::string dir_path = file_path.substr(0, file_path.rfind('\\'));
        if (dir_path.size() == file_path.size())
            dir_path = file_path.substr(0, file_path.rfind('/'));
        return dir_path + "/scripts_lnk/arena/contenders/Sampy.chai";
    }

    std::string getLocalPathStorage() {
        std::string file_path = __FILE__;
        std::string dir_path = file_path.substr(0, file_path.rfind('\\'));
        if (dir_path.size() == file_path.size())
            dir_path = file_path.substr(0, file_path.rfind('/'));
        return dir_path + "/testCopyTo";
    }

    std::string getCopyPathStorage() {
        std::string file_path = __FILE__;
        std::string dir_path = file_path.substr(0, file_path.rfind('\\'));
        if (dir_path.size() == file_path.size())
            dir_path = file_path.substr(0, file_path.rfind('/'));
        return dir_path + "/scripts_lnk";
    }
}

/**
 * @brief Sampy is a script representing a sample monster and also contains standalone scripts.
 *        It is used in order to test the implementation of ChaiScript int FyS Online.
 */
TEST_CASE("Test Sampy", "[service][arena][script_test]") {

    fys::cache::CmlCopy ccpy(getLocalPathStorage(), getCopyPathStorage());
    std::filesystem::path baseCache = getLocalPathStorage();

    fys::arena::PitContenders pc;
    fys::arena::AllyPartyTeams apt;

    auto chai = fys::arena::ChaiRegister::createChaiInstance(pc, apt);

    fys::arena::ChaiRegister::registerBaseActions(*chai, ccpy);

    fys::arena::ContenderScriptingUPtr sampy = std::make_unique<fys::arena::ContenderScripting>(*chai, 1);
    sampy->setContenderId(0u);
    sampy->setContenderName("Sampy");
    sampy->loadContenderScriptFromFile(getPathSampyChaiScript());

    auto fpc = std::make_shared<fys::arena::FightingContender>(std::move(sampy));
    pc.addContender(fpc);

    std::string userName = "FyS";
    fys::arena::PartyTeamUPtr pt = std::make_unique<fys::arena::PartyTeam>(userName);
    fys::arena::TeamMemberSPtr teamMember1 = std::make_shared<fys::arena::TeamMember>(userName, "fyston1");
    fys::arena::TeamMemberSPtr teamMember2 = std::make_shared<fys::arena::TeamMember>(userName, "fyston2");

    pt->addTeamMember(teamMember1);
    pt->addTeamMember(teamMember2);

    fpc->moveContender(fys::arena::HexagonSide::Orientation::A_N, true);
    teamMember1->moveTeamMember(fys::arena::HexagonSide::Orientation::A_N, true);
    teamMember2->moveTeamMember(fys::arena::HexagonSide::Orientation::A_N, true);

    apt.addPartyTeam(std::move(pt));

    SECTION("Test initialization") {
        REQUIRE(153 == fpc->accessStatus().life.current);
        REQUIRE(8 == pc.getFightingContender(0)->accessStatus().initialSpeed);
        REQUIRE(153 == pc.getFightingContender(0)->accessStatus().life.current);
        REQUIRE(153 == pc.getFightingContender(0)->accessStatus().life.total);
        REQUIRE(100 == pc.getFightingContender(0)->accessStatus().magicPoint.total);
        REQUIRE(100 == pc.getFightingContender(0)->accessStatus().magicPoint.total);
    }

    SECTION("Test Enemy&Attack selection") {
        fpc->accessStatus().life.current = 16;
        fpc->accessStatus().life.total = 100;
        teamMember1->accessStatus().life.current = 10;
        teamMember1->accessStatus().life.total = 100;
        teamMember2->accessStatus().life.current = 90;
        teamMember2->accessStatus().life.total = 110;

        fys::arena::data::PriorityElem e { 0, 1, true };

        SECTION("Test Action selection") {
            fpc->accessStatus().life.current = 100;
            fpc->accessStatus().life.total = 100;
            fpc->accessStatus().magicPoint.total = 0;

            REQUIRE(100 == fpc->accessStatus().life.current);
            REQUIRE(100 == fpc->accessStatus().magicPoint.current);
            pc.executeContenderAction(e);

            REQUIRE(42 == fpc->accessStatus().magicPoint.current);          // Sleeping set the magicPoint to 42


        } // End section : Test baseAttack selection

        SECTION("Test Action Selection BaseAttack") {
            teamMember1->accessStatus().life.current = 10;
            teamMember1->accessStatus().life.total = 100;

            REQUIRE(16 == fpc->accessStatus().life.current);    // 16 < ( 50% of 100hp) so baseAttack is decided (50 damage)
            pc.executeContenderAction(e);

            REQUIRE(0 == teamMember1->accessStatus().life.current); // lower life opponent selected (teamMember1 with 10 life point)
            REQUIRE(teamMember1->accessStatus().life.isDead());

        } // Test Action Selection BaseAttack

        SECTION("Test Selection Change of enemy") {
            teamMember2->accessStatus().life.current = 9;       // teamMember2.life.current < teamMember1.life.current

            REQUIRE(16 == fpc->accessStatus().life.current);    // 16 < ( 50% of 100hp) so baseAttack is decided (50 damage)
            pc.executeContenderAction(e);

            REQUIRE(0 == teamMember2->accessStatus().life.current); // lower life opponent selected (teamMember1 with 10 life point)
            REQUIRE(teamMember2->accessStatus().life.isDead());

        } // Test Action Selection BaseAttack

    } // End section : Test Enemy selection

    SECTION("Test network message") {
        auto fseamMock = FSeam::getDefault<fys::arena::ConnectionHandler>();

//         fseamMock->expectArg<FSeam::ConnectionHandler::sendMessageToDispatcher>(
//             FSeam::CustomComparator<zmq::multipart_t &>([](auto && test) {
//                 // verify multipart message contains the selection of the weakest enemy (teamMember1)
//                 return true;
//             }), FSeam::VerifyCompare{1});
        // REQUIRE(fseamMock->verify(FSeam::ConnectionHandler::sendMessageToDispatcher::NAME, 1));

        FSeam::MockVerifier::cleanUp();
        std::filesystem::remove_all(baseCache);

    } // End of Section : Test network message

} // End TestCase : Test Sampy