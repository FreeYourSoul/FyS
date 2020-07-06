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
#include <fightingPit/data/CommonTypes.hh>

#include <connection_handler.hh>
#include <ChaiRegister.hh>
#include <Cml.hh>
#include <flatbuffer_generator.hh>

using chaiscript::fun;

namespace fys::arena {

void
ChaiRegister::registerChai(chaiscript::ChaiScript& chai, PitContenders& pc, AllyPartyTeams& apt, FightingPitLayout& layout)
{
	try {
		chaiscript::ModulePtr m = std::make_shared<chaiscript::Module>();
		registerCommon(m);
		registerFightingPitContender(chai, m);
		registerTeamAllies(chai, m);
		registerUtility(chai, pc, apt);

		chai.add(m);

		chai.set_global(chaiscript::var(std::ref(pc)), "pitContenders");
		chai.set_global(chaiscript::var(std::ref(apt)), "allyPartyTeams");
		chai.set_global(chaiscript::var(std::ref(layout)), "pitLayout");

	}
	catch (const chaiscript::exception::eval_error& ex) {
		SPDLOG_ERROR("Error caught on script execution:\n{}", ex.what());
	}
}

void
ChaiRegister::registerBaseActions(chaiscript::ChaiScript& chai, cache::Cml& cml)
{
	static const std::vector<std::string> baseActions = {
			"arena:actions:action.chai",
			"arena:actions:damage:damage.chai",
			"arena:actions:heal:heal.chai",
			"arena:actions:zone_damage:zone_damage.chai",
			"arena:actions:zone_heal:zone_heal.chai",
			"arena:contenders:contender_functions.chai"
	};

	for (const auto& actionKey : baseActions) {
		try {
			const std::string actionScript = cml.findInCache(actionKey);
			if (!actionScript.empty()) {
				chai.eval(actionScript);
			}
			else {
				SPDLOG_WARN("The base action {} couldn't be registered; empty script found", actionKey);
			}
		}
		catch (const chaiscript::exception::eval_error& ee) {
			SPDLOG_ERROR("Error while loading key {} :\n{}", actionKey, ee.what());
		}
	}
}

bool
ChaiRegister::loadAndRegisterActionPartyTeam(chaiscript::ChaiScript& chai, cache::Cml& cache, PartyTeam& pt)
{
	try {
		// Load actions and register Members
		for (auto& tm : pt.getTeamMembers()) {
			const auto& actionsDoable = tm->getActionsDoable();

			for (const auto &[key, lvl] : actionsDoable) {

				// load the script with its includes
				loadWithIncludes(chai, cache, std::set{key});

				// instantiate the action variable for given team member in chai engine
				const std::string keyPlayer = std::string(pt.getUserName()).append("_").append(tm->getName());
				const std::string actionName = data::getActionNameFromKey(key);
				std::string createVar = fmt::format(
						R"(ally_actions.insert( ["{}":[ "{}":{}({}) ] ] );)", keyPlayer, actionName, actionName, lvl);
				chai.eval(createVar);
			}
		}
	}
	catch (const chaiscript::exception::eval_error& ee) {
		SPDLOG_ERROR("Error caught on scripting action loading :\n{}", ee.what());
		return false;
	}
	return true;
}

void
ChaiRegister::loadContenderScript(chaiscript::ChaiScript& chai, cache::Cml& cml, const std::string& contenderKey)
{
	loadWithIncludes(chai, cml, std::set<std::string>{contenderKey});
}

void
ChaiRegister::loadWithIncludes(chaiscript::ChaiScript& chai, cache::Cml& cache,
		const std::vector<std::string>& keys, std::set<std::string> incursion)
{
	for (const auto& key : keys) {
		loadScript(chai, cache, key);
		std::string includeRetrieve = data::getActionNameFromKey(key).append("_includes();");
		SPDLOG_DEBUG("Retrieve includes of scripts with key {}, eval {}", key, includeRetrieve);
		try {
			std::vector currentIncludes = chai.eval<std::vector<std::string>>(includeRetrieve);
			if (!currentIncludes.empty() && incursion.insert(key).second) {
				loadWithIncludes(chai, cache, currentIncludes, incursion);
			}
		}
		catch (const std::exception& e) {
			SPDLOG_DEBUG("No include for {}, by evaluating {}: {}", key, includeRetrieve, e.what());
		}
	}
}

void
ChaiRegister::loadWithIncludes(chaiscript::ChaiScript& chai, cache::Cml& cache, const std::set<std::string>& keys)
{
	loadWithIncludes(chai, cache, std::vector(keys.begin(), keys.end()), std::set<std::string>{});
}

void
ChaiRegister::loadScripts(chaiscript::ChaiScript& chai, cache::Cml& cache, const std::vector<std::string>& keys)
{
	for (const auto& key : keys) {
		loadScript(chai, cache, key);
	}
}

bool
ChaiRegister::loadScript(chaiscript::ChaiScript& chai, cache::Cml& cache, const std::string& key)
{
	const std::string& action = cache.findInCache(key);
	if (action.empty()) {
		SPDLOG_ERROR("Action with key {} not found (key may be wrong)", key);
		return false;
	}
	try {
		chai.eval(action);
	}
	catch (const std::exception& e) {
		SPDLOG_DEBUG("Action with key {} may be already loaded: {}", key, e.what());
		return false;
	}
	return true;
}

void
ChaiRegister::registerUtility(chaiscript::ChaiScript& chai, PitContenders& pc, AllyPartyTeams& apt)
{
	chai.add(chaiscript::fun<std::function<double(double, double)> >(
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
					return pc.getFightingContender(id)->getHexagonSide().canMove(side);
				}
				else if (!isContender && id < apt.getNumberAlly()) {
					return apt.selectMemberById(id)->getHexagonSide().canMove(side);
				}
				return false;
			}), "isCharacterOnAdjacentSide");
	chai.add(chaiscript::fun<std::function<bool(HexagonSide::Orientation, HexagonSide::Orientation)> >(
			[](HexagonSide::Orientation lhs, HexagonSide::Orientation rhs) {
				return HexagonSide{lhs}.canMove(rhs);
			}), "isSideAdjacentSide");
}

void
ChaiRegister::registerCommon(chaiscript::ModulePtr m)
{

	m->add(chaiscript::fun<std::function<void(fys::arena::data::Status&, std::vector<data::Alteration>, bool)>>(
			[](fys::arena::data::Status& status, std::vector<data::Alteration> alterations, bool replaceIfExist) {
				data::mergeAlterations(status.alterations, std::move(alterations), replaceIfExist);
			}), "addOnTurnAlterations");
	m->add(chaiscript::fun<std::function<void(fys::arena::data::Status&, std::vector<data::Alteration>, bool)>>(
			[](fys::arena::data::Status& status, std::vector<data::Alteration> alterations, bool replaceIfExist) {
				data::mergeAlterations(status.alteration_before, std::move(alterations), replaceIfExist);
			}), "addBeforeTurnAlterations");
	m->add(chaiscript::fun<std::function<void(fys::arena::data::Status&, std::vector<data::Alteration>, bool)>>(
			[](fys::arena::data::Status& status, std::vector<data::Alteration> alterations, bool replaceIfExist) {
				data::mergeAlterations(status.alteration_after, std::move(alterations), replaceIfExist);
			}), "addAfterTurnAlterations");

	chaiscript::utility::add_class<fys::arena::FightingPitLayout>(
			*m,
			"FightingPitLayout",
			{},
			{
					{fun(&fys::arena::FightingPitLayout::initiateContenderMove), "initiateContenderMove"},
					{fun(&fys::arena::FightingPitLayout::initiateMemberMove), "initiateMemberMove"},
					{fun(&fys::arena::FightingPitLayout::initiateForceContenderMove), "initiateForceContenderMove"},
					{fun(&fys::arena::FightingPitLayout::initiateForceMemberMove), "initiateForceMemberMove"},
					{fun(&fys::arena::FightingPitLayout::initiateContenderMoveDir), "initiateContenderMoveDir"},
					{fun(&fys::arena::FightingPitLayout::initiateMemberMoveDir), "initiateMemberMoveDir"}
			});

	chaiscript::utility::add_class<fys::arena::data::Targeting>(
			*m,
			"Targeting",
			{
					{fys::arena::data::SELF, "SELF"},
					{fys::arena::data::ALLY, "ALLY"},
					{fys::arena::data::ALLIES, "ALLIES"},
					{fys::arena::data::ENNEMIES, "ENNEMIES"},
					{fys::arena::data::ENNEMY, "ENNEMY"},
					{fys::arena::data::SIDE, "SIDE"},
					{fys::arena::data::ALLY_OR_ENNEMY, "ALLY_OR_ENNEMY"},
					{fys::arena::data::ALLY_AND_ENNEMY, "ALLY_AND_ENNEMY"}
			});

	chaiscript::utility::add_class<fys::arena::data::Alteration>(
			*m,
			"Alteration",
			{
					{chaiscript::constructor<
							fys::arena::data::Alteration(
									std::string alterationKey,
									uint lvl,
									uint turn,
									std::function<int(data::Status&, uint, uint)>)>()
					}
			},
			{
					{fun(&fys::arena::data::Alteration::processAlteration), "processAlteration"}
			}
	);

	chaiscript::utility::add_class<fys::arena::data::Life>(
			*m, "Life", {},
			{
					{fun(&data::Life::current), "current"},
					{fun(&data::Life::total), "total"},
					{fun(&data::Life::isDead), "isDead"}
			});

	chaiscript::utility::add_class<fys::arena::data::MagicPoint>(
			*m, "MagicPoint", {},
			{
					{fun(&data::MagicPoint::current), "current"},
					{fun(&data::MagicPoint::total), "total"}
			});

	chaiscript::utility::add_class<fys::arena::data::Status>(
			*m, "Status", {},
			{
					{fun(&data::Status::life), "life"},
					{fun(&data::Status::magicPoint), "magicPoint"},
					{fun(&data::Status::initialSpeed), "speed"},
			});

	chaiscript::utility::add_class<fys::arena::HexagonSide::Orientation>(
			*m,
			"Orientation",
			{
					{fys::arena::HexagonSide::Orientation::A_N, "A_N"},
					{fys::arena::HexagonSide::Orientation::A_NE, "A_NE"},
					{fys::arena::HexagonSide::Orientation::A_NW, "A_NW"},
					{fys::arena::HexagonSide::Orientation::A_S, "A_S"},
					{fys::arena::HexagonSide::Orientation::A_SE, "A_SE"},
					{fys::arena::HexagonSide::Orientation::A_SW, "A_SW"},
					{fys::arena::HexagonSide::Orientation::B_N, "B_N"},
					{fys::arena::HexagonSide::Orientation::B_NE, "B_NE"},
					{fys::arena::HexagonSide::Orientation::B_NW, "B_NW"},
					{fys::arena::HexagonSide::Orientation::B_S, "B_S"},
					{fys::arena::HexagonSide::Orientation::B_SE, "B_SE"},
					{fys::arena::HexagonSide::Orientation::B_SW, "B_SW"},
					{fys::arena::HexagonSide::Orientation::C_N, "C_N"},
					{fys::arena::HexagonSide::Orientation::C_NE, "C_NE"},
					{fys::arena::HexagonSide::Orientation::C_NW, "C_NW"},
					{fys::arena::HexagonSide::Orientation::C_S, "C_S"},
					{fys::arena::HexagonSide::Orientation::C_SE, "C_SE"},
					{fys::arena::HexagonSide::Orientation::C_SW, "C_SW"},
					{fys::arena::HexagonSide::Orientation::NONE, "NONE"}
			});

	chaiscript::utility::add_class<fys::arena::HexagonSide::Hexagon>(
			*m,
			"Hexagon",
			{
					{fys::arena::HexagonSide::Hexagon::A, "A"},
					{fys::arena::HexagonSide::Hexagon::B, "B"},
					{fys::arena::HexagonSide::Hexagon::C, "C"}
			});

}

void
ChaiRegister::registerFightingPitContender(chaiscript::ChaiScript& chai, chaiscript::ModulePtr m)
{
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
	chai.add(chaiscript::vector_conversion<std::vector<std::string>>());
	chai.add(chaiscript::vector_conversion<std::vector<data::Alteration>>());

	chaiscript::utility::add_class<fys::arena::PitContenders>(
			*m, "PitContenders", {},
			{
					{fun(&PitContenders::selectSuitableContenderOnSide), "selectSuitableContenderOnSide"},
					{fun(&PitContenders::selectSuitableContender), "selectSuitableContender"},
					{fun(&PitContenders::selectSuitableContenderOnSideAlive), "selectSuitableContenderOnSideAlive"},
					{fun(&PitContenders::selectSuitableContenderAlive), "selectSuitableContenderAlive"},
					{fun(&PitContenders::selectRandomContenderOnSideAlive), "selectRandomContenderAliveOnSide"},
					{fun(&PitContenders::getFightingContender), "getFightingContender"},
					{fun(&PitContenders::getContenderOnSide), "getContenderOnSide"}
			}
	);
}

void
ChaiRegister::registerTeamAllies(chaiscript::ChaiScript& chai, chaiscript::ModulePtr m)
{

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
					{fun(&AllyPartyTeams::getMembersBySide), "getMembersBySide"},
					{fun(&AllyPartyTeams::selectSuitableMemberOnSide), "selectSuitableMemberOnSide"},
					{fun(&AllyPartyTeams::selectSuitableMember), "selectSuitableMember"},
					{fun(&AllyPartyTeams::selectSuitableMemberOnSideAlive), "selectSuitableMemberOnSideAlive"},
					{fun(&AllyPartyTeams::selectSuitableMemberAlive), "selectSuitableMemberAlive"},
					{fun(&AllyPartyTeams::selectRandomMemberOnSideAlive), "selectRandomMemberOnSideAlive"}
			}
	);

	// instantiate global map that contains the action for each given player's ally the doable actions
	chai.eval(R"(global ally_actions = [ "":"" ];)");

}

std::unique_ptr<chaiscript::ChaiScript>
ChaiRegister::createChaiInstance(PitContenders& pc, AllyPartyTeams& apt, FightingPitLayout& layout)
{
	auto chai = std::make_unique<chaiscript::ChaiScript>();
	registerChai(*chai, pc, apt, layout);
	return chai;
}

void
ChaiRegister::registerNetworkCommands(chaiscript::ChaiScript& chai, std::function<void(zmq::message_t&&)> networkHandler)
{
	chai.add(fun<std::function<void(const std::string&, const std::vector<TeamMemberSPtr>&)>>(
			[networkHandler = std::move(networkHandler)](
					const std::string& actionKey,
					const std::vector<TeamMemberSPtr>& allyTargets) {
				flatbuffer_generator fg;
				auto[data, size] = fg.generateActionNotification(actionKey, {}, allyTargets);
				networkHandler(zmq::message_t(data, size));
			}
	), "broadcastActionExecuted");

	chai.add(fun<std::function<void(const std::string&, const std::vector<FightingContenderSPtr>&)>>(
			[networkHandler = std::move(networkHandler)](
					const std::string& actionKey,
					const std::vector<FightingContenderSPtr>& contenderTargets) {
				flatbuffer_generator fg;
				auto[data, size] = fg.generateActionNotification(actionKey, contenderTargets, {});
				networkHandler(zmq::message_t(data, size));
			}
	), "broadcastActionExecuted");
}

}
