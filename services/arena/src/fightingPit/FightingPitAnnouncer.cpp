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

#include <nlohmann/json.hpp>

#include <algorithm/algorithm.hh>
#include <Cml.hh>
#include <ArenaServerContext.hh>
#include <ConnectionHandler.hh>
#include <RandomGenerator.hh>
#include <fightingPit/contender/ContenderScripting.hh>
#include <fightingPit/contender/FightingContender.hh>
#include <fightingPit/team/PartyTeam.hh>
#include <fightingPit/team/TeamMember.hh>
#include <fightingPit/FightingPitAnnouncer.hh>
#include <fightingPit/FightingPit.hh>
#include <BoundaryMap.hh>

namespace {
using BoundaryMapEncounter = fys::BoundaryMap<fys::arena::EncounterContext::EncounterDesc>;

BoundaryMapEncounter
makeContenderRngBoundaryMap(const std::vector<fys::arena::EncounterContext::EncounterDesc>& zoneContenders,
							fys::arena::FightingPit::Level difficulty)
{
	BoundaryMapEncounter bm;
	int val = 0;
	for (const auto& zc : zoneContenders) {
		val += zc.chance.at(static_cast<int>(difficulty));
		bm.insert(val, zc);
	}
	return bm;
}

std::string
getNameFromKey(const std::string& key)
{
	return key.substr(key.find_last_of(':') + 1, key.find_last_of('.') - key.find_last_of(':') - 1);
}

}

namespace fys::arena {
using json = nlohmann::json;

std::unique_ptr<FightingPit>
FightingPitAnnouncer::buildFightingPit(const EncounterContext& ctx, const std::string& wsId)
{
	if (_creatorUserName.empty() || _creatorUserToken.empty()) {
		SPDLOG_WARN("FightingPit built invalid (no creator of the pit registered, a call to generateAllyPartyTeam function is required)");
		return nullptr;
	}
	std::unique_ptr <FightingPit> fp = std::make_unique<FightingPit>(_creatorUserName, _difficulty);
	fp->addAuthenticatedUser(std::move(_creatorUserName), std::move(_creatorUserToken));
	ChaiRegister::registerBaseActions(*fp->getChaiPtr(), _cache);
	generateContenders(*fp, ctx, wsId);
	fp->initializePartyTeam(generateAllyPartyTeam());
	fp->initializeSideBattles();
	return fp;
}

void
FightingPitAnnouncer::generateContenders(FightingPit& fp, const EncounterContext& ctx,
										 const std::string& wsId)
{
	const auto& range = ctx._rangeEncounterPerZone.at(wsId).at(static_cast<std::size_t>(_difficulty));
	const unsigned numberContenders = fys::util::RandomGenerator::generateInRange(range.first, range.second);
	const auto boundaryMap = makeContenderRngBoundaryMap(ctx._contendersPerZone.at(wsId), _difficulty);

	for (unsigned i = 0; i < numberContenders; ++i) {
		int rngMonster = fys::util::RandomGenerator::generateInRange(0, 100);
		auto desc = boundaryMap.get(rngMonster)->second; // TODO add security at context creation to enforce that
		int levelMonster = fys::util::RandomGenerator::generateInRange(1, 10); // TODO, add this range in the configuration
		auto contenderScript = std::make_unique<ContenderScripting>(*fp.getChaiPtr(), levelMonster);
		std::string name = getNameFromKey(desc.key);
		contenderScript->setContenderId(i);
		contenderScript->setContenderName(name);
		contenderScript->loadContenderScript(getScriptContentString(std::move(name), desc));
		auto contender = std::make_shared<FightingContender>(std::move(contenderScript));
		// todo make positioning of contender depending on ambush
		contender->moveContender(HexagonSide::Orientation::B_S, true);
		fp.addContender(contender);
	}
}

AllyPartyTeams
FightingPitAnnouncer::generateAllyPartyTeam()
{
	AllyPartyTeams apt;
	auto team = std::make_unique<PartyTeam>(_creatorUserName);

	// Temporary hard coded party team
	auto tm1 = std::make_shared<TeamMember>(_creatorUserName, "Elvo");
	auto tm2 = std::make_shared<TeamMember>(_creatorUserName, "Mirael");
	auto tm3 = std::make_shared<TeamMember>(_creatorUserName, "Fyston");
	auto tm4 = std::make_shared<TeamMember>(_creatorUserName, "Simon");

	tm1->moveTeamMember(HexagonSide::Orientation::B_S, true);
	auto& s1 = tm1->accessStatus();
	s1.life.total = 100;
	s1.life.current = 100;
	s1.magicPoint.total = 20;
	s1.magicPoint.current = 20;
	s1.initialSpeed = 3;
	tm2->moveTeamMember(HexagonSide::Orientation::B_S, true);
	auto& s2 = tm2->accessStatus();
	s2.life.total = 200;
	s2.life.current = 200;
	s2.magicPoint.total = 0;
	s2.magicPoint.current = 0;
	s2.initialSpeed = 5;
	tm3->moveTeamMember(HexagonSide::Orientation::B_S, true);
	auto& s3 = tm3->accessStatus();
	s3.life.total = 550;
	s3.life.current = 550;
	s3.magicPoint.total = 10;
	s3.magicPoint.current = 10;
	s3.initialSpeed = 10;
	tm4->moveTeamMember(HexagonSide::Orientation::B_S, true);
	auto& s4 = tm4->accessStatus();
	s4.life.total = 140;
	s4.life.current = 140;
	s4.magicPoint.total = 10;
	s4.magicPoint.current = 10;
	s4.initialSpeed = 20;

	team->addTeamMember(std::move(tm1));
	team->addTeamMember(std::move(tm2));
	team->addTeamMember(std::move(tm3));
	team->addTeamMember(std::move(tm4));

	// TODO get data from DB to initialize party team

	apt.addPartyTeam(std::move(team));
	return apt;
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

} // namespace fys::arena
