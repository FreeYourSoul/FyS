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


#include <spdlog/spdlog.h>
#include <iterator>
#include <fmt/ostream.h>
#include <functional>
#include <random>

#include <chaiscript/chaiscript.hpp>

#include <RandomGenerator.hh>

#include <fightingPit/contender/FightingContender.hh>
#include <fightingPit/contender/PitContenders.hh>
#include <fightingPit/team/TeamMember.hh>
#include <fightingPit/team/AllyPartyTeams.hh>
#include <fightingPit/team/PartyTeam.hh>

#include <ConnectionHandler.hh>
#include <ChaiRegister.hh>
#include <Cml.hh>

using chaiscript::fun;

namespace {
    std::string getActionNameFromKey(const std::string &key) {
        auto startSeparator = key.find_last_of(':');
        return key.substr(startSeparator + 1, key.find_last_of('.') - startSeparator - 1);
    }
}

namespace fys::arena {

    void ChaiRegister::registerChai(chaiscript::ChaiScript &chai, PitContenders& pc, AllyPartyTeams &apt) {

        try {
            chaiscript::ModulePtr m = std::make_shared<chaiscript::Module>();
            registerCommon(m);
            registerFightingPitContender(chai, m);
            registerTeamAllies(chai, m);
            registerUtility(chai, pc, apt);

            chai.add(m);
            chai.set_global(chaiscript::var(std::ref(pc)),  "pitContenders");
            chai.set_global(chaiscript::var(std::ref(apt)), "allyPartyTeams");

        }
        catch (std::exception &ex) {
            SPDLOG_ERROR("Error caught on script execution {}", ex.what());
        }
    }

    void ChaiRegister::registerBaseActions(chaiscript::ChaiScript &chai, cache::Cml &cml) {
        static const std::vector<std::string> baseActions = {
                "arena:actions:damage:damage.chai",
                "arena:actions:heal:heal.chai",
                "arena:actions:zone_damage:zone_damage.chai",
                "arena:actions:zone_heal:zone_heal.chai"
        };

        for (const auto & actionKey : baseActions) {
            try {
                const std::string actionScript = cml.findInCache(actionKey);
                if (!actionScript.empty()) {
                    chai.eval(actionScript);
                }
                else {
                    SPDLOG_WARN("The base action {} couldn't be registered; empty script found", actionKey);
                }
            }
            catch (std::exception & e) {
                SPDLOG_ERROR("Error while loading key {} : {}", actionKey, e.what());
            }
        }
    }

    bool ChaiRegister::loadAndRegisterAction(chaiscript::ChaiScript & chai, cache::Cml & cache, const fys::arena::PartyTeam & pt) {
        try {
            for (const auto & tm : pt.getTeamMembers()) {
                const auto & actionsDoable = tm->getActionsDoable();

                for (const auto & [action, lvl] : actionsDoable) {
                    try {
                        chai.eval(cache.findInCache(action));
                    } catch (...) { SPDLOG_DEBUG("Action {} already loaded", action); }
                    // instantiate the action variable for given team member in chai engine
                    const std::string keyPlayer = std::string(pt.getUserName()).append("_").append(tm->getName());
                    const std::string actionName = getActionNameFromKey(action);
                    std::string createVar = fmt::format(
                            R"(ally_actions.insert( ["{}":[ "{}":{}({}) ] ] );)", keyPlayer, actionName, actionName, lvl);
                    chai.eval(createVar);
                }
            }
        } catch (std::exception &e) {
            SPDLOG_ERROR("Error caught on scripting action loading {}", e.what());
            return false;
        }
        return true;
    }

    void ChaiRegister::registerUtility(chaiscript::ChaiScript &chai, PitContenders& pc, AllyPartyTeams &apt) {

        chai.add(chaiscript::fun<std::function<double (double, double)> >(
                [](double rangeA, double rangeB) {
                    return util::RandomGenerator::generateInRange(rangeA, rangeB);
                }), "generateRandomNumber");
        chai.add(chaiscript::fun<std::function<unsigned(unsigned, unsigned)> >(
                [](unsigned rangeA, unsigned rangeB) {
                    return util::RandomGenerator::generateInRange(rangeA, rangeB);
                }), "generateRandomNumber");

        chai.add(chaiscript::fun<std::function<bool(bool, unsigned, HexagonSide::Orientation)> >(
                [&pc, &apt](bool isContender, unsigned id, HexagonSide::Orientation side) {
                    if (isContender && id < pc.getNumberContender()) {
                        return side == pc.getFightingContender(id)->getHexagonSideOrient();
                    }
                    else if (!isContender && id < apt.getNumberAlly()) {
                        return side == apt.selectMemberById(id)->getHexagonSideOrient();
                    }
                    return false;
                }), "isCharacterOnSide");
        chai.add(chaiscript::fun<std::function<bool(bool, unsigned, HexagonSide::Orientation)> >(
                [&pc, &apt](bool isContender, unsigned id, HexagonSide::Orientation side) {
                    if (isContender && id < pc.getNumberContender()) {
                        return  pc.getFightingContender(id)->getHexagonSide().canMove(side);
                    }
                    else if (!isContender && id < apt.getNumberAlly()) {
                        return apt.selectMemberById(id)->getHexagonSide().canMove(side);
                    }
                    return false;
                }), "isCharacterOnAdjacentSide");
        chai.add(chaiscript::fun<std::function<bool(HexagonSide::Orientation, HexagonSide::Orientation)> >(
                [&pc, &apt](HexagonSide::Orientation lhs, HexagonSide::Orientation rhs) {
                    return HexagonSide{lhs}.canMove(rhs);
                }), "isSideAdjacentSide");
    }

    //            chai.add(fun<std::function<void(uint, uint, std::string)> >(
//                    [&connectionHandler](
//                            uint contenderId,                     // [Mandatory] id of the contender
//                            uint spellId,                         // [Mandatory] Spell/Action id
//                            const std::string &descriptionString  // [Mandatory] Description String (Heal of 15, Damage of 17,
//                                                                  //             Critical Damage of 55, Buff intelligence....)
//                    ) {
//                        // instantiate a message to send
//                        // continue setting values
//                        // send the data to connectionHandler
//
//                    }
//            ), "sendContenderAction"); // No target

//            chai.add(chaiscript::fun<std::function<void(std::string, uint, uint, std::string)> >(
//                    [&connectionHandler](
//                            std::string targetId,                 // [Optional]  Target (if any) TODO : Should be a pair of <bool, uint>
//                            uint contenderId,                     // [Mandatory] id of the contender
//                            uint spellId,                         // [Mandatory] Spell/Action id
//                            const std::string &descriptionString  // [Mandatory] Description String (Heal of 15, Damage of 17, Critical Damage of 55, Buff intelligence....)
//                    ) {
//                        // instantiate a message to send
//                        if (!targetId.empty() && targetId.size() > 1) {
//                            bool isFriendlyTarget = (targetId.at(0) == 'F');
//                            uint idTargetInteger = std::stoi(targetId.substr(1));
//                            // set the id value of the message to send
//                        }
//                        // continue setting values
//                        // send the data to connectionHandler
//
//                    }
//            ), "sendContenderAction");

    void ChaiRegister::registerCommon(chaiscript::ModulePtr m) {

        chaiscript::utility::add_class<fys::arena::data::Targeting>(
                *m,
                "Targeting",
                {
                    { fys::arena::data::SELF, "SELF" },
                    { fys::arena::data::ALLY, "ALLY" },
                    { fys::arena::data::ALLIES, "ALLIES" },
                    { fys::arena::data::ENNEMIES, "ENNEMIES" },
                    { fys::arena::data::ENNEMY, "ENNEMY" },
                    { fys::arena::data::SIDE, "SIDE" },
                    { fys::arena::data::ALLY_AND_ENNEMY, "ALLY_AND_ENNEMY" }
                }
        );

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
                        {fun(&data::Status::life),         "life"},
                        {fun(&data::Status::magicPoint),   "magicPoint"},
                        {fun(&data::Status::initialSpeed), "speed"},
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

    void ChaiRegister::registerFightingPitContender(chaiscript::ChaiScript &chai, chaiscript::ModulePtr m) {
        chaiscript::utility::add_class<fys::arena::FightingContender>(
                *m,
                "FightingContender",
                {},
                {
                        {fun(&FightingContender::getHexagonSideOrient), "getHexagonSideOrient"},
                        {fun(&FightingContender::accessStatus), "accessStatus"}
                }
        );

        chaiscript::bootstrap::standard_library::vector_type<std::vector<std::shared_ptr<FightingContender>>>("VectorFightingContender", *m);
        chai.add(chaiscript::vector_conversion<std::vector<std::shared_ptr<FightingContender>>>());

        chaiscript::utility::add_class<fys::arena::PitContenders>(
                *m, "PitContenders", {},
                {
                        {fun(&PitContenders::selectSuitableContenderOnSide), "selectSuitableContenderOnSide"},
                        {fun(&PitContenders::selectSuitableContender), "selectSuitableContender"},
                        {fun(&PitContenders::selectSuitableContenderOnSideAlive), "selectSuitableContenderOnSideAlive"},
                        {fun(&PitContenders::selectSuitableContenderAlive), "selectSuitableContenderAlive"},
                        {fun(&PitContenders::getFightingContender), "getFightingContender"},
                        {fun(&PitContenders::getContenderOnSide),   "getContenderOnSide"}
                }
        );
    }

    void ChaiRegister::registerTeamAllies(chaiscript::ChaiScript &chai, chaiscript::ModulePtr m) {

            chaiscript::utility::add_class<fys::arena::TeamMember>(
                    *m,
                    "TeamMember",
                    {},
                    {
                            {fun(&TeamMember::accessStatus), "accessStatus"}
                    }
            );

            chaiscript::bootstrap::standard_library::vector_type<std::vector<std::shared_ptr<TeamMember>>>("VectorTeamMember", *m);
            chai.add(chaiscript::vector_conversion<std::vector<std::shared_ptr<TeamMember>>>());

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

            // instantiate global map that contains the action for each given player's ally the doable actions
            chai.eval(R"(global ally_actions = [ "":"" ];)");

    }

    std::unique_ptr<chaiscript::ChaiScript> ChaiRegister::createChaiInstance(PitContenders &pc, AllyPartyTeams &apt) {
        auto chai = std::make_unique<chaiscript::ChaiScript>();
        registerChai(*chai, pc, apt);
        return chai;
    }


}
