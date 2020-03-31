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


#ifndef FYS_FIGHTINGPITANNOUNCER_HH
#define FYS_FIGHTINGPITANNOUNCER_HH

#include <optional>
#include <memory>
#include <vector>
#include <ArenaServerContext.hh>
#include <fightingPit/contender/PitContenders.hh>
#include <fightingPit/team/AllyPartyTeams.hh>
#include <fightingPit/FightingPit.hh>

// forward declarations
namespace fys::cache {
class Cml;
}

namespace fys::arena {

/**
 * @brief Builder class to create the FightingPit.
 *   - It generate the position of the Pit participants (contenders and team)
 *   - Define if the fight is an ambush or not.
 *   - Download the required scripts for the contenders (if not in the cache).
 *   - Download the required scripts for the team (if not in the cache)
 *   - Set the level of the fighting pit
 *
 */
class FightingPitAnnouncer {

public:
	enum EncounterType {
		RANDOM,
		SCRIPTED
	};

public:
	explicit FightingPitAnnouncer(cache::Cml& cml)
			:_cache(cml) { }

	/**
	 * Build a fighting pit thanks to the information given
	 * @param ctx encounter context supplied at the startup of the application (contains informations about the type
	 * of encounter a player can find depending on zones
	 * todo : zones are not only defined by world server id, position may be something that should come back in the communication protocol
	 * @param wsId world server id on which the generation of the fighting pit takes place
	 * @return a newly generated fighting pit
	 */
	[[nodiscard]] std::unique_ptr<FightingPit>
	buildFightingPit(const EncounterContext& ctx, const std::string& wsId);


	void setEncounterType(EncounterType encounterType)
	{
		if (encounterType == RANDOM)
			_idEncounter = 0;
		_encounterType = encounterType;
	}

	void setEncounterId(uint idEncounter)
	{
		if (_encounterType == RANDOM && !idEncounter)
			_encounterType = SCRIPTED;
		_idEncounter = idEncounter;
	}

	/**
	 * It is assumed that the player defined by the username/token is already correctly authenticated before
	 * calling this method (call this method only if the username/token is valid).
	 * @param creatorUserName name of the authenticated player generating a fighting pit
	 */
	void setCreatorUserName(std::string creatorUserName) { _creatorUserName = std::move(creatorUserName); }

	/**
	 * It is assumed that the player defined by the username/token is already correctly authenticated before
	 * calling this method (call this method only if the username/token is valid).
	 * @param creatorUserToken token of the authenticated player generating a fighting pit
	 */
	void setCreatorUserToken(std::string creatorUserToken) { _creatorUserToken = std::move(creatorUserToken); }

	void enforceAmbush(bool ambushEnforced) noexcept { _isAmbushEnforced = ambushEnforced; }
	void setDifficulty(FightingPit::Level level) noexcept { _difficulty = level; }
	void setCreatorTeamParty(std::unique_ptr<PartyTeam> pt) { _creatorPartyTeam = std::move(pt); }

	// for testing validation purpose
	[[nodiscard]] static const AllyPartyTeams&
	getPartyTeams(const std::unique_ptr<FightingPit>& fp) { return fp->_partyTeams; }

	[[nodiscard]] static const PitContenders&
	getPitContenders(const std::unique_ptr<FightingPit>& fp) { return fp->_contenders; }

	[[nodiscard]] static const std::string&
	getCreatorUserName(const std::unique_ptr<FightingPit>& fp) { return fp->_creatorUserName; }

	[[nodiscard]] static SideBattle&
	getSideBattleForSide(const std::unique_ptr<FightingPit>& fp, HexagonSide::Orientation side);

	[[nodiscard]] static unsigned
	getArenaId(const std::unique_ptr<FightingPit>& fp) { return fp->_arenaId; }

	// just for testing purpose
	void addActionToOneMember(uint index, const std::string& actionName, uint level);

private:
	[[nodiscard]] inline const std::string&
	getScriptContentString(std::string name, const EncounterContext::EncounterDesc& desc);

	[[nodiscard]] bool
	isScriptedEncounter() const { return _encounterType != EncounterType::RANDOM; }

	[[nodiscard]] bool
	isRandomEncounter() const { return _encounterType == EncounterType::RANDOM; }

	[[nodiscard]] bool
	generateContenders(FightingPit& fp, const EncounterContext& ctx, const std::string& wsId);

private:
	cache::Cml& _cache;

	/**
	 *   range           desc
	 *     0    : random encounter generation
	 * [1001-*] : globally defined encounters
	 * [1-1000] : zone defined encounters
	 */
	uint _idEncounter = 0;
	FightingPit::Level _difficulty = FightingPit::Level::MEDIUM;
	EncounterType _encounterType = EncounterType::RANDOM;

	std::optional<bool> _isAmbushEnforced;

	std::vector<std::string> _loadedScript;

	std::string _creatorUserName;
	std::string _creatorUserToken;
	std::unique_ptr<PartyTeam> _creatorPartyTeam;

	//TODO add the common cache retriever

};

}

#endif // !FYS_FIGHTINGPITANNOUNCER_HH
