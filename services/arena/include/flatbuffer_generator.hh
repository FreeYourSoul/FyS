// MIT License
//
// Copyright (c) 2021-2022 Quentin Balland
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

#include <flatbuffers/flatbuffers.h>
#include <utility>

namespace fys::fb::arn {
struct FightingPitState;
struct PartyTeamStatus;
struct MemberStatus;
struct Cosmetics;
struct Reward;
}// namespace fys::fb::arn
namespace fys::arena {
class ally_party_teams;
class party_team;
class team_member;
class fighting_contender;
class fighting_pit;
struct rewards;

using fighting_contender_sptr = std::shared_ptr<fighting_contender>;
using team_member_sptr = std::shared_ptr<team_member>;

}// namespace fys::arena

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
class flatbuffer_generator {

public:
  [[nodiscard]] std::pair<void*, std::uint32_t>
  generateErrorSaturated(const std::string& arenaCode);

  [[nodiscard]] std::pair<void*, std::uint32_t>
  generateFightingPitState(const fys::arena::fighting_pit& fp);

  [[nodiscard]] std::pair<void*, std::uint32_t>
  generate_party_team_status(const fys::arena::party_team& partyTeam);

  [[nodiscard]] std::pair<void*, std::uint32_t>
  generate_end_battle(bool win, const rewards& rewards);

  [[nodiscard]] std::pair<void*, std::uint32_t>
  generate_action_notification(const std::string& action_key,
                               const std::vector<fighting_contender_sptr>& contender_targets,
                               const std::vector<team_member_sptr>& ally_targets);

  //	[[nodiscard]] std::pair<void*, std::uint32_t>
  //	generateCosmetics(const fys::arena::PartyTeam& partyTeam);
  //
  //	[[nodiscard]] std::pair<void*, std::uint32_t>
  //	generateCosmetics(const std::vector<PitContenders>& contenders);

private:
  [[nodiscard]] inline std::vector<flatbuffers::Offset<fb::arn::PartyTeamStatus>>
  generate_party_team_vec_status_offset(const ally_party_teams& apt);

  [[nodiscard]] inline std::vector<flatbuffers::Offset<fb::arn::MemberStatus>>
  generate_team_member_vec_status_offset(const std::vector<team_member_sptr>& members);

  [[nodiscard]] inline std::vector<flatbuffers::Offset<fys::fb::arn::MemberStatus>>
  generate_contender_vec_status_offset(const std::vector<fighting_contender_sptr>& contenders);

  [[nodiscard]] inline std::vector<flatbuffers::Offset<fb::arn::Reward>>
  generate_rewards_offset(const rewards& rewards);

  //	[[nodiscard]] inline flatbuffers::Offset<fys::fb::arn::Cosmetics>
  //	generateCosmeticsOffset(const fys::arena::PartyTeam&);
  //
  //	[[nodiscard]] inline flatbuffers::Offset<fys::fb::arn::Cosmetics>
  //	generateCosmeticsOffset(const std::vector<fys::arena::FightingContender>& contenders);

private:
  flatbuffers::FlatBufferBuilder _fbb;
};

}// namespace fys::arena
#endif//FYS_ONLINE_ARN_FLATBUFFERGENERATOR_HH
