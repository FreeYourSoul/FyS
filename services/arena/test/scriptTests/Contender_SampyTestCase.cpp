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
#include <FSeamMockData.hpp>
#include <fightingPit/contender/ContenderScripting.hh>
#include <fightingPit/contender/PitContenders.hh>
#include <fightingPit/team/AllyPartyTeams.hh>
#include <fightingPit/team/PartyTeam.hh>
#include <fightingPit/team/TeamMember.hh>

/**
 * @brief Sampy is a script representing a sample monster and also contains standalone scripts.
 *        It is used in order to test the implementation of ChaiScript int FyS Online.
 */

TEST_CASE("Test Sampy", "[script_test]") {
    
    auto fseamMock = FSeam::getDefault<fys::arena::ConnectionHandler>();

    fys::arena::ContenderScriptingUPtr sampy = std::make_unique<fys::arena::ContenderScripting>();
    sampy->loadContenderScriptFromFile("./scripts/contenders/Sampy.chai");

    fys::arena::PitContenders pc;
    auto fpc = fys::arena::FightingPitContender::makeFightingContender(std::move(sampy));

    fys::arena::AllyPartyTeams apt;
    fys::arena::PartyTeamUPtr pt = std::make_unique<fys::arena::PartyTeam>();
    fys::arena::TeamMemberSPtr teamMember1 = std::make_shared<fys::arena::TeamMember>();
    fys::arena::TeamMemberSPtr teamMember2 = std::make_shared<fys::arena::TeamMember>();
    
    PartyTeamUPtr->addPartyTeam(std::move(teamMember1));
    PartyTeamUPtr->addPartyTeam(std::move(teamMember2));

    SECTION("Test Enemy selection") {
        teamMember1->accessStatus.life.current = 10;
        teamMember1->accessStatus.life.total = 100;
        teamMember2->accessStatus.life.current = 90;
        teamMember2->accessStatus.life.total = 110;
        
        fseamMock->expectArg<FSeam::ConnectionHandler::sendMessageToDispatcher>(
            CustomComparator<zmq::multipart_t>([](auto && test){
                // verify multipart message contains the selection of the weakest enemy (teamMember1)
                return true;
            }), 1);

        pc.executeAction(pc, apt);

        REQUIRE(fseamMock->verify(FSeam::ConnectionHandler::sendMessageToDispatcher::NAME, 1));

    } // End section : Test Enemy selection

    SECTION("Test action selection") {

        fseamMock->expectArg<FSeam::ConnectionHandler::sendMessageToDispatcher>(
            CustomComparator<zmq::multipart_t>([](auto && test){
                // verify multipart message contains the selection of a specific action
                return true;
            }), 1);

        pc.executeAction(pc, apt);


        fseamMock->expectArg<FSeam::ConnectionHandler::sendMessageToDispatcher>(
            CustomComparator<zmq::multipart_t>([](auto && test){
                // verify multipart message contains the selection of a specific action
                return true;
            }), 1);

        pc.executeAction(pc, apt);

        REQUIRE(fseamMock->verify(FSeam::ConnectionHandler::sendMessageToDispatcher::NAME));

    } // End section : 

    FSeam::MockVerifier::cleanUp();

} // End TestCase : Test Sampy
