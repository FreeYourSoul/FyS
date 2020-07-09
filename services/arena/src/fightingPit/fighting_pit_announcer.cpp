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

#include <chaiscript/chaiscript.hpp>
#include <nlohmann/json.hpp>
#include <algorithm/algorithm.hh>

#include <fightingPit/contender/contender_scripting.hh>
#include <fightingPit/contender/fighting_contender.hh>
#include <fightingPit/team/party_team.hh>
#include <fightingPit/team/team_member.hh>
#include <fightingPit/fighting_pit.hh>

#include <Cml.hh>
#include <arena_server_context.hh>
#include <connection_handler.hh>
#include <random_generator.hh>
#include <boundary_map.hh>

#include <fightingPit/fighting_pit_announcer.hh>

namespace {
using boundaray_map_encounter = fys::boundary_map<fys::arena::encounter_context::encounter_desc>;
using boundary_map_reward = fys::boundary_map<std::string>;

[[nodiscard]] boundaray_map_encounter
make_contender_rng_boundary_map(const std::vector<fys::arena::encounter_context::encounter_desc>& zone_contenders,
		fys::arena::fighting_pit::level difficulty)
{
	boundaray_map_encounter bm;
	unsigned val = 0;
	for (const auto& zc : zone_contenders) {
		val += zc.chance.at(static_cast<uint>(difficulty));
		bm.insert(val, zc);
	}
	return bm;
}

[[nodiscard]] boundary_map_reward
make_reward_rng_boundary_map(const fys::arena::encounter_context::reward_encounter_desc& red,
		fys::arena::fighting_pit::level difficulty)
{
	boundary_map_reward bm;
	unsigned val = 0;
	for (const auto&[item_key, chance] : red.item_on_chance_range) {
		val += chance.at(static_cast<uint>(difficulty));
		bm.insert(val, item_key);
	}
	return bm;
}

}

namespace fys::arena {
using json = nlohmann::json;

std::unique_ptr<fighting_pit>
fighting_pit_announcer::build_fighting_pit(const encounter_context& ctx, const std::string& ws_id)
{
	if (_creator_user_name.empty() || _creator_user_token.empty() || _creator_party_team == nullptr) {
		SPDLOG_WARN("FightingPit built invalid (no creator/token of the pit registered and/or party team not set"
					", a call to generateAllyPartyTeam function is required)");
		return nullptr;
	}
	if (!ctx.zoneRegistered(ws_id)) {
		SPDLOG_WARN("FightingPit built invalid the zone of world server {} isn't registered", ws_id);
		return nullptr;
	}
	std::unique_ptr<fighting_pit> fp = std::make_unique<fighting_pit>(_creator_user_name, _difficulty);
	fp->add_authenticated_user(std::move(_creator_user_name), std::move(_creator_user_token));
	if (_is_join_disabled) {
		fp->disable_join();
	}
	chai_register::register_base_actions(*fp->get_chai_ptr(), _cache);
	fp->add_party_team_and_register_actions(std::move(_creator_party_team), _cache);

	if (!generate_contenders(*fp, ctx, ws_id)) {
		return nullptr;
	}

	generate_reward_for_contender(*fp, ctx, fp->contenders().contenders());

	fp->initialize_side_battles();

	return fp;
}

bool
fighting_pit_announcer::generate_contenders(fighting_pit& fp, const encounter_context& ctx,
		const std::string& wsId)
{
	const auto& range = ctx._rangeEncounterPerZone.at(wsId).at(static_cast<std::size_t>(_difficulty));
	const unsigned number_contenders = fys::util::random_generator::generate_in_range(range.first, range.second);
	const auto boundary_map = make_contender_rng_boundary_map(ctx._contendersPerZone.at(wsId), _difficulty);

	for (unsigned i = 0; i < number_contenders; ++i) {
		int rng_monster = util::random_generator::generate_in_range(0, 100);
		auto desc = boundary_map.get(rng_monster)->second;
		std::uint32_t level_monster = util::random_generator::generate_in_range(desc.levelRange.first, desc.levelRange.second);
		auto contender_script = std::make_unique<contender_scripting>(*fp.get_chai_ptr(), level_monster);
		std::string name = data::get_action_name_from_key(desc.key);
		contender_script->set_contender_id(i);
		contender_script->set_contender_name(name);
		chai_register::load_contender_script(*fp.get_chai_ptr(), _cache, desc.key);
		contender_script->register_contender_script();
		auto contender = std::make_shared<fighting_contender>(std::move(contender_script));
		if (!fp.add_contender(contender)) {
			SPDLOG_WARN("FightingPit built invalid, generation of contender {} failed", contender->name());
			return false;
		}
		// todo make positioning of contender depending on ambush / or normal one
		fighting_pit_layout::set_contender_initiate_position(*contender, hexagon_side::orientation::B_S);
	}
	return true;
}

void
fighting_pit_announcer::generate_reward_for_contender(fighting_pit& fp,
		const encounter_context& ctx, const std::vector<fighting_contender_sptr>& contenders)
{
	std::map<std::string, uint> reward_on_quantity;
	for (const auto& contender : contenders) {
		const auto& rwd_it = ctx._rewardDescPerContender.find(contender->name());
		// Check if the contender has a reward description
		if (rwd_it == ctx._rewardDescPerContender.end())
			continue;

		boundary_map_reward boundaryMap = make_reward_rng_boundary_map(rwd_it->second, _difficulty);
		const auto& range_rwd_number = rwd_it->second.rangeDrop.at(static_cast<std::size_t>(_difficulty));
		const std::uint32_t number_reward = util::random_generator::generate_in_range(range_rwd_number.first, range_rwd_number.second);

		for (std::uint32_t i = 0; i < number_reward; ++i) {
			const std::uint32_t reward_rng = util::random_generator::generate_in_range(0u, 100u);
			++reward_on_quantity[boundaryMap.get(reward_rng)->second];
		}
	}

	for (auto&[key_reward, quantity] : reward_on_quantity) {
		fp.add_rewards(std::move(key_reward), quantity);
	}
}

const std::string&
fighting_pit_announcer::get_script_content_string(std::string name, const encounter_context::encounter_desc& desc)
{
	static const std::string empty{};
	if (std::any_of(_loaded_script.cbegin(), _loaded_script.cend(), [&name](const auto& s) { return s == name; }))
		return empty;

	_loaded_script.emplace_back(std::move(name));
	return _cache.findInCache(desc.key);
}

side_battle&
fighting_pit_announcer::get_side_battle_for_side(const std::unique_ptr<fighting_pit>& fp, hexagon_side::orientation side)
{
	auto it = std::find_if(fp->_side_battles.begin(), fp->_side_battles.end(),
			[side](const auto& sb) { return sb.side() == side; });

	if (it == fp->_side_battles.end()) {
		SPDLOG_ERROR("Side not found");
		return fp->_side_battles.front();
	}
	return *it;
}

void
fighting_pit_announcer::add_action_to_one_member(std::uint32_t index, const std::string& action_name, std::uint32_t level)
{
	_creator_party_team->access_team_members()[index]->add_doable_action(action_name, level);
}

} // namespace fys::arena
