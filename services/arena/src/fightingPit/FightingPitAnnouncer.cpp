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

#include <fightingPit/contender/ContenderScripting.hh>
#include <fightingPit/contender/FightingContender.hh>
#include <fightingPit/team/PartyTeam.hh>
#include <fightingPit/team/TeamMember.hh>
#include <fightingPit/FightingPit.hh>

#include <Cml.hh>
#include <ArenaServerContext.hh>
#include <ConnectionHandler.hh>
#include <RandomGenerator.hh>
#include <BoundaryMap.hh>

#include <fightingPit/FightingPitAnnouncer.hh>

namespace {
using BoundaryMapEncounter = fys::BoundaryMap<fys::arena::EncounterContext::EncounterDesc>;
using BoundaryMapReward = fys::BoundaryMap<std::string>;

[[nodiscard]] BoundaryMapEncounter
makeContenderRngBoundaryMap(const std::vector<fys::arena::EncounterContext::EncounterDesc>& zoneContenders,
		fys::arena::FightingPit::Level difficulty)
{
	BoundaryMapEncounter bm;
	unsigned val = 0;
	for (const auto& zc : zoneContenders) {
		val += zc.chance.at(static_cast<uint>(difficulty));
		bm.insert(val, zc);
	}
	return bm;
}

[[nodiscard]] BoundaryMapReward
makeRewardRngBoundaryMap(const fys::arena::EncounterContext::RewardEncounterDesc& red,
		fys::arena::FightingPit::Level difficulty)
{
	BoundaryMapReward bm;
	unsigned val = 0;
	for (const auto&[itemKey, chance] : red.itemOnChanceRange) {
		val += chance.at(static_cast<uint>(difficulty));
		bm.insert(val, itemKey);
	}
	return bm;
}

}

namespace fys::arena {
using json = nlohmann::json;

std::unique_ptr<FightingPit>
FightingPitAnnouncer::buildFightingPit(const EncounterContext& ctx, const std::string& wsId)
{
	if (_creatorUserName.empty() || _creatorUserToken.empty() || _creatorPartyTeam == nullptr) {
		SPDLOG_WARN("FightingPit built invalid (no creator/token of the pit registered and/or party team not set"
					", a call to generateAllyPartyTeam function is required)");
		return nullptr;
	}
	if (!ctx.zoneRegistered(wsId)) {
		SPDLOG_WARN("FightingPit built invalid the zone of world server {} isn't registered", wsId);
		return nullptr;
	}
	std::unique_ptr<FightingPit> fp = std::make_unique<FightingPit>(_creatorUserName, _difficulty);
	fp->addAuthenticatedUser(std::move(_creatorUserName), std::move(_creatorUserToken));
	if (_isJoinDisabled) {
		fp->disableJoin();
	}
	ChaiRegister::registerBaseActions(*fp->getChaiPtr(), _cache);
	fp->addPartyTeamAndRegisterActions(std::move(_creatorPartyTeam), _cache);

	if (!generateContenders(*fp, ctx, wsId)) {
		return nullptr;
	}

	generateRewardForContender(*fp, ctx, fp->getPitContenders().getContenders());

	fp->initializeSideBattles();

	return fp;
}

bool
FightingPitAnnouncer::generateContenders(FightingPit& fp, const EncounterContext& ctx,
		const std::string& wsId)
{
	const auto& range = ctx._rangeEncounterPerZone.at(wsId).at(static_cast<std::size_t>(_difficulty));
	const unsigned numberContenders = fys::util::RandomGenerator::generateInRange(range.first, range.second);
	const auto boundaryMap = makeContenderRngBoundaryMap(ctx._contendersPerZone.at(wsId), _difficulty);

	for (unsigned i = 0; i < numberContenders; ++i) {
		int rngMonster = util::RandomGenerator::generateInRange(0, 100);
		auto desc = boundaryMap.get(rngMonster)->second;
		uint levelMonster = util::RandomGenerator::generateInRange(desc.levelRange.first, desc.levelRange.second);
		auto contenderScript = std::make_unique<ContenderScripting>(*fp.getChaiPtr(), levelMonster);
		std::string name = data::getActionNameFromKey(desc.key);
		contenderScript->setContenderId(i);
		contenderScript->setContenderName(name);
		ChaiRegister::loadContenderScript(*fp.getChaiPtr(), _cache, desc.key);
		contenderScript->registerContenderScript();
		auto contender = std::make_shared<FightingContender>(std::move(contenderScript));
		if (!fp.addContender(contender)) {
			SPDLOG_WARN("FightingPit built invalid, generation of contender {} failed", contender->getName());
			return false;
		}
		// todo make positioning of contender depending on ambush / or normal one
		fys::arena::FightingPitLayout::setContenderInitiatePosition(*contender, HexagonSide::Orientation::B_S);
	}
	return true;
}

void
FightingPitAnnouncer::generateRewardForContender(FightingPit& fp,
		const EncounterContext& ctx, const std::vector<FightingContenderSPtr>& contenders)
{
	std::map<std::string, uint> rewardOnQuantity;
	for (const auto& contender : contenders) {
		const auto& rwdIt = ctx._rewardDescPerContender.find(contender->getName());
		// Check if the contender has a reward description
		if (rwdIt == ctx._rewardDescPerContender.end())
			continue;

		BoundaryMapReward boundaryMap = makeRewardRngBoundaryMap(rwdIt->second, _difficulty);
		const auto& rangeRwdNumber = rwdIt->second.rangeDrop.at(static_cast<std::size_t>(_difficulty));
		const uint numberReward = util::RandomGenerator::generateInRange(rangeRwdNumber.first, rangeRwdNumber.second);

		for (uint i = 0; i < numberReward; ++i) {
			const uint rewardRng = util::RandomGenerator::generateInRange(0u, 100u);
			++rewardOnQuantity[boundaryMap.get(rewardRng)->second];
		}
	}

	for (auto&[keyReward, quantity] : rewardOnQuantity) {
		fp.addReward(std::move(keyReward), quantity);
	}
}

const std::string&
FightingPitAnnouncer::getScriptContentString(std::string name, const EncounterContext::EncounterDesc& desc)
{
	static const std::string empty{};
	if (std::any_of(_loadedScript.cbegin(), _loadedScript.cend(), [&name](const auto& s) { return s == name; }))
		return empty;

	_loadedScript.emplace_back(std::move(name));
	return _cache.findInCache(desc.key);
}

SideBattle&
FightingPitAnnouncer::getSideBattleForSide(const std::unique_ptr<FightingPit>& fp, HexagonSide::Orientation side)
{
	auto it = std::find_if(fp->_sideBattles.begin(), fp->_sideBattles.end(),
			[side](const auto& sb) { return sb.getSide() == side; });

	if (it == fp->_sideBattles.end()) {
		SPDLOG_ERROR("Side not found");
		return fp->_sideBattles.front();
	}
	return *it;
}

void
FightingPitAnnouncer::addActionToOneMember(uint index, const std::string& actionName, uint level)
{
	_creatorPartyTeam->accessTeamMembers()[index]->addDoableAction(actionName, level);
}

} // namespace fys::arena
