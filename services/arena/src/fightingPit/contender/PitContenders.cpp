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

using namespace chaiscript;

namespace fys::arena {

    chaiscript::ChaiScript &PitContenders::setupScriptEngine(AllyPartyTeams &apt) {
        chaiscript::ModulePtr m = std::make_shared<chaiscript::Module>();

        registerCommon(m);
        registerChaiPitContender(m);
        registerChaiAllies(m);
        _chai.add(m);
        _chai.set_global(chaiscript::var(std::ref(*this)), "pitContenders");
        _chai.set_global(chaiscript::var(std::ref(apt)), "allyPartyTeams");
        return _chai;
    }

    void PitContenders::registerCommon(chaiscript::ModulePtr m) {

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

    void PitContenders::registerChaiAllies(chaiscript::ModulePtr m) {

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

    void PitContenders::registerChaiPitContender(chaiscript::ModulePtr m) {

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
        _chai.add(chaiscript::vector_conversion<std::vector<std::shared_ptr<FightingContender>>>());

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

    std::vector<std::shared_ptr<FightingContender>> PitContenders::getChangingSideContenders() const {
        std::vector<std::shared_ptr<FightingContender>> result;
        result.reserve(_changeSideFlags.size());
        for (std::size_t i = 0; i < _changeSideFlags.size(); ++i) {
            if (_changeSideFlags.at(i)) {
                result.emplace_back(_contenders.at(i));
            }
        }
        return result;
    }

    std::vector<std::shared_ptr<FightingContender>>
    PitContenders::getContenderOnSide(HexagonSide::Orientation side) const {
        std::vector<std::shared_ptr<FightingContender>> result;
        std::copy_if(_contenders.begin(), _contenders.end(), result.begin(), [side](const auto &contenderPtr) {
            return contenderPtr->getHexagonSide().second == side;
        });
        return result;
    }

    std::shared_ptr<FightingContender>
    PitContenders::selectSuitableContenderOnSide(HexagonSide::Orientation side, ComparatorSelection<FightingContender> comp) {
        std::vector<std::shared_ptr<FightingContender>> result;
        auto it = fys::find_most_suitable(_contenders.begin(), _contenders.end(), [side, &comp](auto &current, auto &next){
            return current->getHexagonSideOrient() == side && comp(current, next);
        });
        if (it == _contenders.end())
            return nullptr;
        return *it;
    }

    std::shared_ptr<FightingContender>
    PitContenders::selectSuitableContenderOnSideAlive(HexagonSide::Orientation side, ComparatorSelection<FightingContender> comp) {
        std::vector<std::shared_ptr<FightingContender>> result;
        auto it = fys::find_most_suitable(_contenders.begin(), _contenders.end(), [side, &comp](auto &current, auto &next){
            return !current->accessStatus().life.isDead() && current->getHexagonSideOrient() == side && comp(current, next);
        });
        if (it == _contenders.end())
            return nullptr;
        return *it;
    }

    std::shared_ptr<FightingContender>
    PitContenders::selectSuitableContender(ComparatorSelection<FightingContender> comp) {
        std::vector<std::shared_ptr<FightingContender>> result;
        auto it = fys::find_most_suitable(_contenders.begin(), _contenders.end(), comp);
        if (it == _contenders.end())
            return nullptr;
        return *it;
    }

    std::shared_ptr<FightingContender>
    PitContenders::selectSuitableContenderAlive(ComparatorSelection<FightingContender> comp) {
        std::vector<std::shared_ptr<FightingContender>> result;
        auto it = fys::find_most_suitable(_contenders.begin(), _contenders.end(), [&comp](auto &current, auto &next){
            return !current->accessStatus().life.isDead() && comp(current, next);
        });
        if (it == _contenders.end())
            return nullptr;
        return *it;
    }

    void PitContenders::executeContenderAction(const data::PriorityElem &contender) {
        if (!contender.isContender && contender.id < _contenders.size()) {
            return;
        }
        _contenders.at(contender.id)->executeAction();
    }

}
