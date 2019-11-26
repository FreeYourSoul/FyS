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


#include <functional>

#include <algorithm/algorithm.hh>

#include <fightingPit/contender/FightingContender.hh>
#include <fightingPit/contender/PitContenders.hh>
#include <fightingPit/team/TeamMember.hh>
#include <fightingPit/team/AllyPartyTeams.hh>

#include <ConnectionHandler.hh>
#include <ChaiRegister.hh>

namespace fys::arena {

void ChaiRegister::registerChai(chaiscript::chai &chai, PitContenders& pc, AllyPartyTeams &apt) {
    chaiscript::ModulePtr m = std::make_shared<chaiscript::Module>();

    registerCommon(m);
    registerFightingPitContender(m);
    registerTeamAllies(m);

    chai.add(m);   
    chai.set_global(chaiscript::var(std::ref(*this)), "pitContenders");
    chai.set_global(chaiscript::var(std::ref(apt)), "allyPartyTeams");
}

void ChaiRegister::registerUtility(chaiscript::chai &chai, ConnectionHandler &connectionHandler) {

        _chai.get().add(chaiscript::fun<std::function<void(std::string, uint, uint, std::string)> >([&connectionHandler] (
                    uint contenderId,                     // [Mandatory] id of the contender
                    uint spellId,                         // [Mandatory] Spell/Action id
                    const std::string &descriptionString  // [Mandatory] Description String (Heal of 15, Damage of 17, Critical Damage of 55, Buff intelligence....)
                ) {
                // instantiate a message to send
                // continue setting values
                // send the data to connectionHandler

            }
        ), getChaiMethodName("sendContenderAction")); // No target

        _chai.get().add(chaiscript::fun<std::function<void(std::string, uint, uint, std::string)> >([&connectionHandler] (
                    std::string targetId,                 // [Optional]  Target (if any) TODO : Should be a pair of <bool, uint>
                    uint contenderId,                     // [Mandatory] id of the contender
                    uint spellId,                         // [Mandatory] Spell/Action id
                    const std::string &descriptionString  // [Mandatory] Description String (Heal of 15, Damage of 17, Critical Damage of 55, Buff intelligence....)
                ) {
                // instantiate a message to send
                if (!targetId.empty() && targetId.size() > 1) {
                    bool isFriendlyTarget = (targetId.at(0) == 'F');
                    uint idTargetInteger = std::stoi(targetId.substr(1));
                    // set the id value of the message to send
                }
                // continue setting values
                // send the data to connectionHandler

            }
        ), getChaiMethodName("sendContenderAction"));

        _chai.get().add(chaiscript::fun<std::function<unsigned ()> >([]() {
            return std::rand() % 100;
        }), "generateRandomNumber");
}

void ChaiRegister::registerCommon(chaiscript::ModulePtr m) {

        chaiscript::utility::add_class<fys::arena::data::Life>(
                *m, "Life", {},
                {
                        {fun(&data::Life::current), "current"},
                        {fun(&data::Life::total),   "total"},
                        {fun(&data::Life::isDead),  "isDead"}
                }
        );

        chaiscript::utility::add_class<fys::arena::data::MagicPoint>(
                *m, "MagicPoint", {},
                {
                        {fun(&data::MagicPoint::current), "current"},
                        {fun(&data::MagicPoint::total),   "total"}
                }
        );

        chaiscript::utility::add_class<fys::arena::data::Status>(
                *m, "Status", {},
                {
                        {fun(&data::Status::life),       "life"},
                        {fun(&data::Status::magicPoint), "magicPoint"},
                }
        );

        chaiscript::utility::add_class<fys::arena::HexagonSide::Orientation>(
                *m,
                "Orientation",
                {
                        {fys::arena::HexagonSide::Orientation::A_N,  "A_N"},
                        {fys::arena::HexagonSide::Orientation::A_NE, "A_NE"},
                        {fys::arena::HexagonSide::Orientation::A_NW, "A_NW"},
                        {fys::arena::HexagonSide::Orientation::A_S,  "A_S"},
                        {fys::arena::HexagonSide::Orientation::A_SE, "A_SE"},
                        {fys::arena::HexagonSide::Orientation::A_SW, "A_SW"},
                        {fys::arena::HexagonSide::Orientation::B_N,  "B_N"},
                        {fys::arena::HexagonSide::Orientation::B_NE, "B_NE"},
                        {fys::arena::HexagonSide::Orientation::B_NW, "B_NW"},
                        {fys::arena::HexagonSide::Orientation::B_S,  "B_S"},
                        {fys::arena::HexagonSide::Orientation::B_SE, "B_SE"},
                        {fys::arena::HexagonSide::Orientation::B_SW, "B_SW"},
                        {fys::arena::HexagonSide::Orientation::C_N,  "C_N"},
                        {fys::arena::HexagonSide::Orientation::C_NE, "C_NE"},
                        {fys::arena::HexagonSide::Orientation::C_NW, "C_NW"},
                        {fys::arena::HexagonSide::Orientation::C_S,  "C_S"},
                        {fys::arena::HexagonSide::Orientation::C_SE, "C_SE"},
                        {fys::arena::HexagonSide::Orientation::C_SW, "C_SW"},
                        {fys::arena::HexagonSide::Orientation ::NONE,"NONE"}
                }
        );

        chaiscript::utility::add_class<fys::arena::HexagonSide::Hexagon>(
                *m,
                "Hexagon",
                {
                        {fys::arena::HexagonSide::Hexagon::A, "A"},
                        {fys::arena::HexagonSide::Hexagon::B, "B"},
                        {fys::arena::HexagonSide::Hexagon::C, "C"}
                }
        );
}

void ChaiRegister::registerFightingPitContender(chaiscript::ModulePtr m) {
        
        chaiscript::utility::add_class<fys::arena::data::Life>(
                *m, "Life", {},
                {
                        {fun(&data::Life::current), "current"},
                        {fun(&data::Life::total),   "total"},
                        {fun(&data::Life::isDead),  "isDead"}
                }
        );

        chaiscript::utility::add_class<fys::arena::data::MagicPoint>(
                *m, "MagicPoint", {},
                {
                        {fun(&data::MagicPoint::current), "current"},
                        {fun(&data::MagicPoint::total),   "total"}
                }
        );

        chaiscript::utility::add_class<fys::arena::data::Status>(
                *m, "Status", {},
                {
                        {fun(&data::Status::life),       "life"},
                        {fun(&data::Status::magicPoint), "magicPoint"},
                }
        );

        chaiscript::utility::add_class<fys::arena::HexagonSide::Orientation>(
                *m,
                "Orientation",
                {
                        {fys::arena::HexagonSide::Orientation::A_N,  "A_N"},
                        {fys::arena::HexagonSide::Orientation::A_NE, "A_NE"},
                        {fys::arena::HexagonSide::Orientation::A_NW, "A_NW"},
                        {fys::arena::HexagonSide::Orientation::A_S,  "A_S"},
                        {fys::arena::HexagonSide::Orientation::A_SE, "A_SE"},
                        {fys::arena::HexagonSide::Orientation::A_SW, "A_SW"},
                        {fys::arena::HexagonSide::Orientation::B_N,  "B_N"},
                        {fys::arena::HexagonSide::Orientation::B_NE, "B_NE"},
                        {fys::arena::HexagonSide::Orientation::B_NW, "B_NW"},
                        {fys::arena::HexagonSide::Orientation::B_S,  "B_S"},
                        {fys::arena::HexagonSide::Orientation::B_SE, "B_SE"},
                        {fys::arena::HexagonSide::Orientation::B_SW, "B_SW"},
                        {fys::arena::HexagonSide::Orientation::C_N,  "C_N"},
                        {fys::arena::HexagonSide::Orientation::C_NE, "C_NE"},
                        {fys::arena::HexagonSide::Orientation::C_NW, "C_NW"},
                        {fys::arena::HexagonSide::Orientation::C_S,  "C_S"},
                        {fys::arena::HexagonSide::Orientation::C_SE, "C_SE"},
                        {fys::arena::HexagonSide::Orientation::C_SW, "C_SW"},
                        {fys::arena::HexagonSide::Orientation ::NONE,"NONE"}
                }
        );

        chaiscript::utility::add_class<fys::arena::HexagonSide::Hexagon>(
                *m,
                "Hexagon",
                {
                        {fys::arena::HexagonSide::Hexagon::A, "A"},
                        {fys::arena::HexagonSide::Hexagon::B, "B"},
                        {fys::arena::HexagonSide::Hexagon::C, "C"}
                }
        );
}

void ChaiRegister::registerTeamAllies(chaiscript::ModulePtr m) {
    
        chaiscript::utility::add_class<fys::arena::TeamMember>(
                *m,
                "TeamMember",
                {},
                {
                        {fun(&TeamMember::accessStatus), "accessStatus"}
                }
        );

        chaiscript::bootstrap::standard_library::vector_type<std::vector<std::shared_ptr<TeamMember>>>("VectorTeamMember", *m);
        _chai.add(chaiscript::vector_conversion<std::vector<std::shared_ptr<TeamMember>>>());

        chaiscript::utility::add_class<fys::arena::AllyPartyTeams>(
                *m,
                "AllyPartyTeams",
                {},
                {
                        {fun(&AllyPartyTeams::selectSuitableMemberOnSide), "selectSuitableMemberOnSide"},
                        {fun(&AllyPartyTeams::selectSuitableMember), "selectSuitableMember"},
                        {fun(&AllyPartyTeams::selectSuitableMemberOnSideAlive), "selectSuitableMemberOnSideAlive"},
                        {fun(&AllyPartyTeams::selectSuitableMemberAlive), "selectSuitableMemberAlive"}
                }
        );

}


    
}
