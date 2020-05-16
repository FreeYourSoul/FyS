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


#ifndef FYS_ONLINE_ARN_FLATBUFFERGENERATOR_HH
#define FYS_ONLINE_ARN_FLATBUFFERGENERATOR_HH

#include <utility>
#include <flatbuffers/flatbuffers.h>

namespace fys::fb::arn {
struct FightingPitState;
struct PartyTeamStatus;
struct MemberStatus;
struct Cosmetics;
struct Reward;
}
namespace fys::arena {
class AllyPartyTeams;
class PartyTeam;
class TeamMember;
class FightingContender;
class FightingPit;
struct Rewards;

using FightingContenderSPtr = std::shared_ptr<FightingContender>;
using TeamMemberSPtr = std::shared_ptr<TeamMember>;

}

namespace fys::arena {

/**
 * Class used to generate the FlatBuffer messages using the FightingPit data
 * @note It is important to note that the flatbuffer retrieved from a public member is invalidated
 * if another call is made
 *
 * @example
 * @code
 * FlatbufferGenerator fg;
 *
 * auto fightingPitStateFlatBuffer = generateFightingPitState(fightingPit);
 * auto partyTeamStatusFlatBuffer = generatePartyTeamStatus(partyTeam); // invalidate fightingPitStateFlatBuffer
 *
 * // fightingPitStateFlatBuffer is invalidating fightingPitStateFlatBuffer
 * // any usage of fightingPitStateFlatBuffer is undefined behavior
 *
 * @endcode
 *
 */
class FlatbufferGenerator {

public:
	[[nodiscard]] std::pair<void*, uint>
	generateErrorSaturated(const std::string& arenaCode);

	[[nodiscard]] std::pair<void*, uint>
	generateFightingPitState(const fys::arena::FightingPit& fp);

	[[nodiscard]] std::pair<void*, uint>
	generatePartyTeamStatus(const fys::arena::PartyTeam& partyTeam);

	[[nodiscard]] std::pair<void*, uint>
	generateEndBattle(bool win, const Rewards& rewards);

	[[nodiscard]] std::pair<void*, uint>
	generateActionNotification(const std::string& actionKey,
			const std::vector<FightingContenderSPtr>& contenderTargets,
			const std::vector<TeamMemberSPtr>& allyTargets);

//	[[nodiscard]] std::pair<void*, uint>
//	generateCosmetics(const fys::arena::PartyTeam& partyTeam);
//
//	[[nodiscard]] std::pair<void*, uint>
//	generateCosmetics(const std::vector<PitContenders>& contenders);

private:
	[[nodiscard]] inline std::vector<flatbuffers::Offset<fb::arn::PartyTeamStatus>>
	generatePartyTeamVecStatusOffset(const AllyPartyTeams& apt);

	[[nodiscard]] inline std::vector<flatbuffers::Offset<fb::arn::MemberStatus>>
	generateTeamMemberVecStatusOffset(const std::vector<TeamMemberSPtr>& members);

	[[nodiscard]] inline std::vector<flatbuffers::Offset<fys::fb::arn::MemberStatus>>
	generateContenderVecStatusOffset(const std::vector<FightingContenderSPtr>& contenders);

	[[nodiscard]] inline std::vector<flatbuffers::Offset<fb::arn::Reward>>
	generateRewardsOffset(const Rewards& rewards);

//	[[nodiscard]] inline flatbuffers::Offset<fys::fb::arn::Cosmetics>
//	generateCosmeticsOffset(const fys::arena::PartyTeam&);
//
//	[[nodiscard]] inline flatbuffers::Offset<fys::fb::arn::Cosmetics>
//	generateCosmeticsOffset(const std::vector<fys::arena::FightingContender>& contenders);

private:
	flatbuffers::FlatBufferBuilder _fbb;

};

}
#endif //FYS_ONLINE_ARN_FLATBUFFERGENERATOR_HH
