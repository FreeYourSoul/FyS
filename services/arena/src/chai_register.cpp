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
#include <fightingPit/contender/pit_contenders.hh>
#include <fightingPit/team/team_member.hh>
#include <fightingPit/team/ally_party_teams.hh>
#include <fightingPit/team/party_team.hh>
#include <fightingPit/data/common_types.hh>

#include <connection_handler.hh>
#include <chai_register.hh>
#include <Cml.hh>
#include <flatbuffer_generator.hh>

using chaiscript::fun;

namespace fys::arena {

void
chai_register::register_chai(chaiscript::ChaiScript& chai, pit_contenders& pc,
		ally_party_teams& apt, fighting_pit_layout& layout)
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
chai_register::load_register_action_party_team(chaiscript::ChaiScript& chai, cache::Cml& cache, party_team& pt)
{
	try {
		// Load actions and register Members
		for (auto& tm : pt.get_team_members()) {
			const auto& actionsDoable = tm->get_actions_doable();

			for (const auto &[key, lvl] : actionsDoable) {

				// load the script with its includes
				load_with_includes(chai, cache, std::set{key});

				// instantiate the action variable for given team member in chai engine
				const std::string keyPlayer = std::string(pt.get_user_name()).append("_").append(tm->get_name());
				const std::string actionName = data::get_action_name_from_key(key);
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
		std::string include_retrieve = data::get_action_name_from_key(key).append("_includes();");
		SPDLOG_DEBUG("Retrieve includes of scripts with key {}, eval {}", key, include_retrieve);
		try {
			std::vector currentIncludes = chai.eval<std::vector<std::string>>(include_retrieve);
			if (!currentIncludes.empty() && incursion.insert(key).second) {
				load_with_includes(chai, cache, currentIncludes, incursion);
			}
		}
		catch (const std::exception& e) {
			SPDLOG_DEBUG("No include for {}, by evaluating {}: {}", key, include_retrieve, e.what());
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
chai_register::register_utility(chaiscript::ChaiScript& chai, pit_contenders& pc, ally_party_teams& apt)
{
	chai.add(chaiscript::fun<std::function<double(double, double)> >(
			[](double rangeA, double rangeB) {
				return util::random_generator::generate_in_range(rangeA, rangeB);
			}), "generateRandomNumber");
	chai.add(chaiscript::fun<std::function<unsigned(unsigned, unsigned)> >(
			[](unsigned rangeA, unsigned rangeB) {
				return util::random_generator::generate_in_range(rangeA, rangeB);
			}), "generateRandomNumber");

	chai.add(chaiscript::fun<std::function<bool(bool, unsigned, hexagon_side::orientation)> >(
			[&pc, &apt](bool isContender, unsigned id, hexagon_side::orientation side) {
				if (isContender && id < pc.getNumberContender()) {
					return side == pc.getFightingContender(id)->get_hexagon_side_orient();
				}
				else if (!isContender && id < apt.get_number_ally()) {
					return side == apt.select_member_by_id(id)->get_hexagon_side_orient();
				}
				return false;
			}), "isCharacterOnSide");
	chai.add(chaiscript::fun<std::function<bool(bool, unsigned, hexagon_side::orientation)> >(
			[&pc, &apt](bool isContender, unsigned id, hexagon_side::orientation side) {
				if (isContender && id < pc.getNumberContender()) {
					return pc.getFightingContender(id)->get_hexagon_side().can_move(side);
				}
				else if (!isContender && id < apt.get_number_ally()) {
					return apt.select_member_by_id(id)->get_hexagon_side().can_move(side);
				}
				return false;
			}), "isCharacterOnAdjacentSide");
	chai.add(chaiscript::fun<std::function<bool(hexagon_side::orientation, hexagon_side::orientation)> >(
			[](hexagon_side::orientation lhs, hexagon_side::orientation rhs) {
				return hexagon_side{lhs}.can_move(rhs);
			}), "isSideAdjacentSide");
}

void
chai_register::register_common(chaiscript::ModulePtr m)
{

	m->add(chaiscript::fun<std::function<void(fys::arena::data::status&, std::vector<data::alteration>, bool)>>(
			[](fys::arena::data::status& status, std::vector<data::alteration> alterations, bool replaceIfExist) {
				data::mergeAlterations(status.alterations, std::move(alterations), replaceIfExist);
			}), "addOnTurnAlterations");
	m->add(chaiscript::fun<std::function<void(fys::arena::data::status&, std::vector<data::alteration>, bool)>>(
			[](fys::arena::data::status& status, std::vector<data::alteration> alterations, bool replaceIfExist) {
				data::mergeAlterations(status.alteration_before, std::move(alterations), replaceIfExist);
			}), "addBeforeTurnAlterations");
	m->add(chaiscript::fun<std::function<void(fys::arena::data::status&, std::vector<data::alteration>, bool)>>(
			[](fys::arena::data::status& status, std::vector<data::alteration> alterations, bool replaceIfExist) {
				data::mergeAlterations(status.alteration_after, std::move(alterations), replaceIfExist);
			}), "addAfterTurnAlterations");

	chaiscript::utility::add_class<fys::arena::fighting_pit_layout>(
			*m,
			"FightingPitLayout",
			{},
			{
					{fun(&fys::arena::fighting_pit_layout::initiate_contender_move), "initiateContenderMove"},
					{fun(&fys::arena::fighting_pit_layout::initiate_member_move), "initiateMemberMove"},
					{fun(&fys::arena::fighting_pit_layout::initiate_force_contender_move), "initiateForceContenderMove"},
					{fun(&fys::arena::fighting_pit_layout::initiate_force_member_move), "initiateForceMemberMove"},
					{fun(&fys::arena::fighting_pit_layout::initiate_contender_move_dir), "initiateContenderMoveDir"},
					{fun(&fys::arena::fighting_pit_layout::initiate_member_move_dir), "initiateMemberMoveDir"}
			});

	chaiscript::utility::add_class<fys::arena::data::targeting>(
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

	chaiscript::utility::add_class<fys::arena::data::alteration>(
			*m,
			"Alteration",
			{
					{chaiscript::constructor<
							fys::arena::data::alteration(
									std::string alterationKey,
									uint lvl,
									uint turn,
									std::function<int(data::status&, uint, uint)>)>()
					}
			},
			{
					{fun(&fys::arena::data::alteration::process_alteration), "processAlteration"}
			}
	);

	chaiscript::utility::add_class<fys::arena::data::life>(
			*m, "Life", {},
			{
					{fun(&data::life::current), "current"},
					{fun(&data::life::total), "total"},
					{fun(&data::life::is_dead), "isDead"}
			});

	chaiscript::utility::add_class<fys::arena::data::magic_point>(
			*m, "MagicPoint", {},
			{
					{fun(&data::magic_point::current), "current"},
					{fun(&data::magic_point::total), "total"}
			});

	chaiscript::utility::add_class<fys::arena::data::status>(
			*m, "Status", {},
			{
					{fun(&data::status::life_pt), "life"},
					{fun(&data::status::magic_pt), "magicPoint"},
					{fun(&data::status::initial_speed), "speed"},
			});

	chaiscript::utility::add_class<fys::arena::hexagon_side::orientation>(
			*m,
			"orientation",
			{
					{fys::arena::hexagon_side::orientation::A_N, "A_N"},
					{fys::arena::hexagon_side::orientation::A_NE, "A_NE"},
					{fys::arena::hexagon_side::orientation::A_NW, "A_NW"},
					{fys::arena::hexagon_side::orientation::A_S, "A_S"},
					{fys::arena::hexagon_side::orientation::A_SE, "A_SE"},
					{fys::arena::hexagon_side::orientation::A_SW, "A_SW"},
					{fys::arena::hexagon_side::orientation::B_N, "B_N"},
					{fys::arena::hexagon_side::orientation::B_NE, "B_NE"},
					{fys::arena::hexagon_side::orientation::B_NW, "B_NW"},
					{fys::arena::hexagon_side::orientation::B_S, "B_S"},
					{fys::arena::hexagon_side::orientation::B_SE, "B_SE"},
					{fys::arena::hexagon_side::orientation::B_SW, "B_SW"},
					{fys::arena::hexagon_side::orientation::C_N, "C_N"},
					{fys::arena::hexagon_side::orientation::C_NE, "C_NE"},
					{fys::arena::hexagon_side::orientation::C_NW, "C_NW"},
					{fys::arena::hexagon_side::orientation::C_S, "C_S"},
					{fys::arena::hexagon_side::orientation::C_SE, "C_SE"},
					{fys::arena::hexagon_side::orientation::C_SW, "C_SW"},
					{fys::arena::hexagon_side::orientation::NONE, "NONE"}
			});

	chaiscript::utility::add_class<fys::arena::hexagon_side::hexagon>(
			*m,
			"Hexagon",
			{
					{fys::arena::hexagon_side::hexagon::A, "A"},
					{fys::arena::hexagon_side::hexagon::B, "B"},
					{fys::arena::hexagon_side::hexagon::C, "C"}
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
	chai.add(chaiscript::vector_conversion<std::vector<data::alteration>>());

	chaiscript::utility::add_class<fys::arena::pit_contenders>(
			*m, "PitContenders", {},
			{
					{fun(&pit_contenders::select_suitable_contender_on_side), "selectSuitableContenderOnSide"},
					{fun(&pit_contenders::select_suitable_contender), "selectSuitableContender"},
					{fun(&pit_contenders::select_suitable_contender_on_side_alive), "selectSuitableContenderOnSideAlive"},
					{fun(&pit_contenders::select_suitable_contender_alive), "selectSuitableContenderAlive"},
					{fun(&pit_contenders::select_random_contender_on_side_alive), "selectRandomContenderAliveOnSide"},
					{fun(&pit_contenders::getFightingContender), "getFightingContender"},
					{fun(&pit_contenders::get_contender_on_side), "getContenderOnSide"}
			}
	);
}

void
chai_register::register_team_allies(chaiscript::ChaiScript& chai, chaiscript::ModulePtr m)
{

	chaiscript::utility::add_class<fys::arena::team_member>(
			*m,
			"TeamMember",
			{},
			{
					{fun(&team_member::access_status), "accessStatus"}
			}
	);

	chaiscript::bootstrap::standard_library::vector_type<std::vector<std::shared_ptr<team_member>>>("VectorTeamMember", *m);
	chai.add(chaiscript::vector_conversion<std::vector<std::shared_ptr<team_member>>>());

	chaiscript::utility::add_class<fys::arena::ally_party_teams>(
			*m,
			"AllyPartyTeams",
			{},
			{
					{fun(&ally_party_teams::get_members_by_side), "getMembersBySide"},
					{fun(&ally_party_teams::select_suitable_member_on_side), "selectSuitableMemberOnSide"},
					{fun(&ally_party_teams::select_suitable_member), "selectSuitableMember"},
					{fun(&ally_party_teams::select_suitable_member_on_side_alive), "selectSuitableMemberOnSideAlive"},
					{fun(&ally_party_teams::select_suitable_member_alive), "selectSuitableMemberAlive"},
					{fun(&ally_party_teams::select_random_member_on_side_alive), "selectRandomMemberOnSideAlive"}
			}
	);

	// instantiate global map that contains the action for each given player's ally the doable actions
	chai.eval(R"(global ally_actions = [ "":"" ];)");

}

std::unique_ptr<chaiscript::ChaiScript>
chai_register::make_chai_instance(pit_contenders& pc, ally_party_teams& apt, fighting_pit_layout& layout)
{
	auto chai = std::make_unique<chaiscript::ChaiScript>();
	register_chai(*chai, pc, apt, layout);
	return chai;
}

void
chai_register::register_network_commands(chaiscript::ChaiScript& chai, std::function<void(zmq::message_t&&)> networkHandler)
{
	chai.add(fun<std::function<void(const std::string&, const std::vector<team_member_sptr>&)>>(
			[networkHandler = std::move(networkHandler)](
					const std::string& actionKey,
					const std::vector<team_member_sptr>& allyTargets) {
				flatbuffer_generator fg;
				auto[data, size] = fg.generate_action_notification(actionKey, {}, allyTargets);
				networkHandler(zmq::message_t(data, size));
			}
	), "broadcastActionExecuted");

	chai.add(fun<std::function<void(const std::string&, const std::vector<fighting_contender_sptr>&)>>(
			[networkHandler = std::move(networkHandler)](
					const std::string& actionKey,
					const std::vector<fighting_contender_sptr>& contenderTargets) {
				flatbuffer_generator fg;
				auto[data, size] = fg.generate_action_notification(actionKey, contenderTargets, {});
				networkHandler(zmq::message_t(data, size));
			}
	), "broadcastActionExecuted");
}

}
