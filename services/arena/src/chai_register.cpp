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

#include <random_generator.hh>

#include <fightingPit/contender/fighting_contender.hh>
#include <fightingPit/contender/PitContenders.hh>
#include <fightingPit/team/TeamMember.hh>
#include <fightingPit/team/AllyPartyTeams.hh>
#include <fightingPit/team/PartyTeam.hh>
#include <fightingPit/data/CommonTypes.hh>

#include <connection_handler.hh>
#include <chai_register.hh>
#include <Cml.hh>
#include <flatbuffer_generator.hh>

using chaiscript::fun;

namespace fys::arena {

void
chai_register::register_chai(chaiscript::ChaiScript& chai, PitContenders& pc, AllyPartyTeams& apt, FightingPitLayout& layout)
{
	try {
		chaiscript::ModulePtr m = std::make_shared<chaiscript::Module>();
		register_common(m);
		register_fighting_pit_contender(chai, m);
		register_team_allies(chai, m);
		register_utility(chai, pc, apt);

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
chai_register::register_base_actions(chaiscript::ChaiScript& chai, cache::Cml& cml)
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
chai_register::load_register_action_party_team(chaiscript::ChaiScript& chai, cache::Cml& cache, PartyTeam& pt)
{
	try {
		// Load actions and register Members
		for (auto& tm : pt.getTeamMembers()) {
			const auto& actionsDoable = tm->getActionsDoable();

			for (const auto &[key, lvl] : actionsDoable) {

				// load the script with its includes
				load_with_includes(chai, cache, std::set{key});

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
chai_register::load_contender_script(chaiscript::ChaiScript& chai, cache::Cml& cml, const std::string& contenderKey)
{
	load_with_includes(chai, cml, std::set<std::string>{contenderKey});
}

void
chai_register::load_with_includes(chaiscript::ChaiScript& chai, cache::Cml& cache,
		const std::vector<std::string>& keys, std::set<std::string> incursion)
{
	for (const auto& key : keys) {
		load_scripts(chai, cache, key);
		std::string includeRetrieve = data::getActionNameFromKey(key).append("_includes();");
		SPDLOG_DEBUG("Retrieve includes of scripts with key {}, eval {}", key, includeRetrieve);
		try {
			std::vector currentIncludes = chai.eval<std::vector<std::string>>(includeRetrieve);
			if (!currentIncludes.empty() && incursion.insert(key).second) {
				load_with_includes(chai, cache, currentIncludes, incursion);
			}
		}
		catch (const std::exception& e) {
			SPDLOG_DEBUG("No include for {}, by evaluating {}: {}", key, includeRetrieve, e.what());
		}
	}
}

void
chai_register::load_with_includes(chaiscript::ChaiScript& chai, cache::Cml& cache, const std::set<std::string>& keys)
{
	load_with_includes(chai, cache, std::vector(keys.begin(), keys.end()), std::set<std::string>{});
}

void
chai_register::load_scripts(chaiscript::ChaiScript& chai, cache::Cml& cache, const std::vector<std::string>& keys)
{
	for (const auto& key : keys) {
		load_scripts(chai, cache, key);
	}
}

bool
chai_register::load_scripts(chaiscript::ChaiScript& chai, cache::Cml& cache, const std::string& keys)
{
	const std::string& action = cache.findInCache(keys);
	if (action.empty()) {
		SPDLOG_ERROR("Action with key {} not found (key may be wrong)", keys);
		return false;
	}
	try {
		chai.eval(action);
	}
	catch (const std::exception& e) {
		SPDLOG_DEBUG("Action with key {} may be already loaded: {}", keys, e.what());
		return false;
	}
	return true;
}

void
chai_register::register_utility(chaiscript::ChaiScript& chai, PitContenders& pc, AllyPartyTeams& apt)
{
	chai.add(chaiscript::fun<std::function<double(double, double)> >(
			[](double rangeA, double rangeB) {
				return util::random_generator::generate_in_range(rangeA, rangeB);
			}), "generateRandomNumber");
	chai.add(chaiscript::fun<std::function<unsigned(unsigned, unsigned)> >(
			[](unsigned rangeA, unsigned rangeB) {
				return util::random_generator::generate_in_range(rangeA, rangeB);
			}), "generateRandomNumber");

	chai.add(chaiscript::fun<std::function<bool(bool, unsigned, hexagon_side::Orientation)> >(
			[&pc, &apt](bool isContender, unsigned id, hexagon_side::Orientation side) {
				if (isContender && id < pc.getNumberContender()) {
					return side == pc.getFightingContender(id)->get_hexagon_side_orient();
				}
				else if (!isContender && id < apt.getNumberAlly()) {
					return side == apt.selectMemberById(id)->getHexagonSideOrient();
				}
				return false;
			}), "isCharacterOnSide");
	chai.add(chaiscript::fun<std::function<bool(bool, unsigned, hexagon_side::Orientation)> >(
			[&pc, &apt](bool isContender, unsigned id, hexagon_side::Orientation side) {
				if (isContender && id < pc.getNumberContender()) {
					return pc.getFightingContender(id)->getHexagonSide().canMove(side);
				}
				else if (!isContender && id < apt.getNumberAlly()) {
					return apt.selectMemberById(id)->getHexagonSide().canMove(side);
				}
				return false;
			}), "isCharacterOnAdjacentSide");
	chai.add(chaiscript::fun<std::function<bool(hexagon_side::Orientation, hexagon_side::Orientation)> >(
			[](hexagon_side::Orientation lhs, hexagon_side::Orientation rhs) {
				return hexagon_side{lhs}.canMove(rhs);
			}), "isSideAdjacentSide");
}

void
chai_register::register_common(chaiscript::ModulePtr m)
{

	m->add(chaiscript::fun<std::function<void(fys::arena::data::status&, std::vector<data::Alteration>, bool)>>(
			[](fys::arena::data::status& status, std::vector<data::Alteration> alterations, bool replaceIfExist) {
				data::mergeAlterations(status.alterations, std::move(alterations), replaceIfExist);
			}), "addOnTurnAlterations");
	m->add(chaiscript::fun<std::function<void(fys::arena::data::status&, std::vector<data::Alteration>, bool)>>(
			[](fys::arena::data::status& status, std::vector<data::Alteration> alterations, bool replaceIfExist) {
				data::mergeAlterations(status.alteration_before, std::move(alterations), replaceIfExist);
			}), "addBeforeTurnAlterations");
	m->add(chaiscript::fun<std::function<void(fys::arena::data::status&, std::vector<data::Alteration>, bool)>>(
			[](fys::arena::data::status& status, std::vector<data::Alteration> alterations, bool replaceIfExist) {
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
									std::function<int(data::status&, uint, uint)>)>()
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

	chaiscript::utility::add_class<fys::arena::data::status>(
			*m, "Status", {},
			{
					{fun(&data::status::life), "life"},
					{fun(&data::status::magicPoint), "magicPoint"},
					{fun(&data::status::initialSpeed), "speed"},
			});

	chaiscript::utility::add_class<fys::arena::hexagon_side::Orientation>(
			*m,
			"Orientation",
			{
					{fys::arena::hexagon_side::Orientation::A_N, "A_N"},
					{fys::arena::hexagon_side::Orientation::A_NE, "A_NE"},
					{fys::arena::hexagon_side::Orientation::A_NW, "A_NW"},
					{fys::arena::hexagon_side::Orientation::A_S, "A_S"},
					{fys::arena::hexagon_side::Orientation::A_SE, "A_SE"},
					{fys::arena::hexagon_side::Orientation::A_SW, "A_SW"},
					{fys::arena::hexagon_side::Orientation::B_N, "B_N"},
					{fys::arena::hexagon_side::Orientation::B_NE, "B_NE"},
					{fys::arena::hexagon_side::Orientation::B_NW, "B_NW"},
					{fys::arena::hexagon_side::Orientation::B_S, "B_S"},
					{fys::arena::hexagon_side::Orientation::B_SE, "B_SE"},
					{fys::arena::hexagon_side::Orientation::B_SW, "B_SW"},
					{fys::arena::hexagon_side::Orientation::C_N, "C_N"},
					{fys::arena::hexagon_side::Orientation::C_NE, "C_NE"},
					{fys::arena::hexagon_side::Orientation::C_NW, "C_NW"},
					{fys::arena::hexagon_side::Orientation::C_S, "C_S"},
					{fys::arena::hexagon_side::Orientation::C_SE, "C_SE"},
					{fys::arena::hexagon_side::Orientation::C_SW, "C_SW"},
					{fys::arena::hexagon_side::Orientation::NONE, "NONE"}
			});

	chaiscript::utility::add_class<fys::arena::hexagon_side::Hexagon>(
			*m,
			"Hexagon",
			{
					{fys::arena::hexagon_side::Hexagon::A, "A"},
					{fys::arena::hexagon_side::Hexagon::B, "B"},
					{fys::arena::hexagon_side::Hexagon::C, "C"}
			});

}

void
chai_register::register_fighting_pit_contender(chaiscript::ChaiScript& chai, chaiscript::ModulePtr m)
{
	chaiscript::utility::add_class<fys::arena::fighting_contender>(
			*m,
			"FightingContender",
			{},
			{
					{fun(&fighting_contender::get_hexagon_side_orient), "getHexagonSideOrient"},
					{fun(&fighting_contender::access_status), "accessStatus"}
			}
	);

	chaiscript::bootstrap::standard_library::vector_type<std::vector<std::shared_ptr<fighting_contender>>>("VectorFightingContender", *m);
	chai.add(chaiscript::vector_conversion<std::vector<std::shared_ptr<fighting_contender>>>());
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
chai_register::register_team_allies(chaiscript::ChaiScript& chai, chaiscript::ModulePtr m)
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
chai_register::make_chai_instance(PitContenders& pc, AllyPartyTeams& apt, FightingPitLayout& layout)
{
	auto chai = std::make_unique<chaiscript::ChaiScript>();
	register_chai(*chai, pc, apt, layout);
	return chai;
}

void
chai_register::register_network_commands(chaiscript::ChaiScript& chai, std::function<void(zmq::message_t&&)> networkHandler)
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

	chai.add(fun<std::function<void(const std::string&, const std::vector<fighting_contender_sptr>&)>>(
			[networkHandler = std::move(networkHandler)](
					const std::string& actionKey,
					const std::vector<fighting_contender_sptr>& contenderTargets) {
				flatbuffer_generator fg;
				auto[data, size] = fg.generateActionNotification(actionKey, contenderTargets, {});
				networkHandler(zmq::message_t(data, size));
			}
	), "broadcastActionExecuted");
}

}
