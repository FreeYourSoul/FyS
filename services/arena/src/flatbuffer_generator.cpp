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

#include <FightingPitState_generated.h>
#include <ReplyFrame_generated.h>

#include <fightingPit/contender/contender_scripting.hh>
#include <fightingPit/contender/fighting_contender.hh>
#include <fightingPit/fighting_pit.hh>
#include <fightingPit/rewards.hh>
#include <fightingPit/team/ally_party_teams.hh>
#include <fightingPit/team/party_team.hh>
#include <fightingPit/team/team_member.hh>

#include <util/FbUtility.hh>

#include "../../inventory/include/flatbuffer_generator.hh"
#include "../../worldserver/include/flatbuffer_generator.hh"
#include "flatbuffer_generator.hh"

namespace {

[[nodiscard]] std::vector<std::string>
retrieve_string_vector(const std::vector<std::pair<std::string, uint>>& doableAttacks) {
  std::vector<std::string> stringVec;
  stringVec.reserve(doableAttacks.size());
  for (const auto& [atkString, atkLvl] : doableAttacks) {
    stringVec.emplace_back(atkString);
  }
  return stringVec;
}

}// namespace

namespace fys::arena {

std::pair<void*, std::uint32_t>
flatbuffer_generator::generateErrorSaturated(const std::string& arenaCode) {
  _fbb.Clear();
  auto errorFb = fb::arn::CreateReplyFrame(
      _fbb,
      fys::fb::arn::Content_ErrorMessage,
      _fbb.CreateString(std::string("Arena server is saturated: Code:") + arenaCode).Union());
  fb::arn::FinishReplyFrameBuffer(_fbb, errorFb);
  return std::pair(_fbb.GetBufferPointer(), _fbb.GetSize());
}

std::pair<void*, std::uint32_t>
flatbuffer_generator::generateFightingPitState(const fys::arena::fighting_pit& fp) {
  _fbb.Clear();
  auto fbPartyTeamVec = _fbb.CreateVector(generate_party_team_vec_status_offset(fp.ally_party()));
  auto fbContenderVec = _fbb.CreateVector(generate_contender_vec_status_offset(fp.contenders().get_contenders()));
  auto fps = fb::arn::CreateFightingPitState(
      _fbb,
      fp.id(),
      fbPartyTeamVec,
      fbContenderVec);
  auto message = fb::arn::CreateReplyFrame(
      _fbb,
      fys::fb::arn::Content_FightingPitState,
      fps.Union());
  fb::arn::FinishReplyFrameBuffer(_fbb, message);
  return std::pair(_fbb.GetBufferPointer(), _fbb.GetSize());
}

std::pair<void*, std::uint32_t>
flatbuffer_generator::generate_party_team_status(const party_team& partyTeam) {
  _fbb.Clear();
  auto fb_name = _fbb.CreateString(partyTeam.user_name());
  auto fb_member_vec = _fbb.CreateVector(generate_team_member_vec_status_offset(partyTeam.team_members()));
  auto fb_vec_attacks = [this](const party_team& partyTeam) {
    std::vector<std::string> vecString;
    for (const auto& tm : partyTeam.team_members()) {
      auto vec = retrieve_string_vector(tm->actions_doable());
      std::move(vec.begin(), vec.end(), std::back_inserter(vecString));
    }
    return _fbb.CreateVectorOfStrings(vecString);
  }(partyTeam);

  fb::arn::PartyTeamStatusBuilder builder(_fbb);
  builder.add_user_name(fb_name);
  builder.add_members(fb_member_vec);
  builder.add_attacks(fb_vec_attacks);

  auto message = fb::arn::CreateReplyFrame(
      _fbb,
      fys::fb::arn::Content_PartyTeamStatus,
      builder.Finish().Union());
  fb::arn::FinishReplyFrameBuffer(_fbb, message);
  return std::pair(_fbb.GetBufferPointer(), _fbb.GetSize());
}

std::pair<void*, std::uint32_t>
flatbuffer_generator::generate_end_battle(bool win, const rewards& rewards) {
  _fbb.Clear();
  flatbuffers::Offset<fys::fb::arn::EndBattle> eb;

  if (win) {
    eb = fb::arn::CreateEndBattle(
        _fbb,
        fys::fb::arn::StatusBattle_WON,
        _fbb.CreateVector(generate_rewards_offset(rewards)));
  } else {
    eb = fb::arn::CreateEndBattle(
        _fbb,
        fys::fb::arn::StatusBattle_LOST);
  }
  auto end_battle_fb = fb::arn::CreateReplyFrame(
      _fbb,
      fys::fb::arn::Content_EndBattle,
      eb.Union());
  fb::arn::FinishReplyFrameBuffer(_fbb, end_battle_fb);
  return std::pair(_fbb.GetBufferPointer(), _fbb.GetSize());
}

std::pair<void*, std::uint32_t>
flatbuffer_generator::generate_action_notification(
    const std::string& action_key,
    const std::vector<fighting_contender_sptr>& contender_targets,
    const std::vector<team_member_sptr>& ally_targets) {
  auto contender_status_fb = generate_contender_vec_status_offset(contender_targets);
  auto party_team_status_fb = generate_team_member_vec_status_offset(ally_targets);
  auto ae = fb::arn::CreateActionExecuted(_fbb,
                                          _fbb.CreateString(action_key),
                                          _fbb.CreateVector(party_team_status_fb),
                                          _fbb.CreateVector(contender_status_fb));
  auto end_battle_fb = fb::arn::CreateReplyFrame(
      _fbb,
      fys::fb::arn::Content_ActionExecuted,
      ae.Union());
  fb::arn::FinishReplyFrameBuffer(_fbb, end_battle_fb);
  return std::pair(_fbb.GetBufferPointer(), _fbb.GetSize());
}

//std::pair<void*, uint>
//FlatbufferGenerator::generateCosmetics(const PartyTeam& partyTeam)
//{
//	_fbb.Clear();
//	return std::pair(nullptr, 0);
//}
//
//std::pair<void*, uint>
//FlatbufferGenerator::generateCosmetics(const std::vector<PitContenders>& contenders)
//{
//	_fbb.Clear();
//	return std::pair(nullptr, 0);
//}

// privates

std::vector<flatbuffers::Offset<fb::arn::PartyTeamStatus>>
flatbuffer_generator::generate_party_team_vec_status_offset(const ally_party_teams& apt) {
  std::vector<flatbuffers::Offset<fb::arn::PartyTeamStatus>> fb_party_teams_status;
  const auto& party_teams = apt.get_party_teams();

  fb_party_teams_status.reserve(party_teams.size());
  for (const auto& pt : party_teams) {
    auto fb_user_name = _fbb.CreateString(pt->user_name());
    auto fb_members = _fbb.CreateVector(generate_team_member_vec_status_offset(pt->team_members()));
    auto fb_attacks = [this](const auto& pt) {
      std::vector<std::string> attacks;
      for (const auto& tm : pt->team_members()) {
        auto vec = retrieve_string_vector(tm->actions_doable());
        std::move(vec.begin(), vec.end(), std::back_inserter(attacks));
      }
      return _fbb.CreateVectorOfStrings(attacks);
    }(pt);

    fb::arn::PartyTeamStatusBuilder builder(_fbb);
    builder.add_user_name(fb_user_name);
    builder.add_members(fb_members);
    builder.add_attacks(fb_attacks);
    fb_party_teams_status.emplace_back(builder.Finish());
  }
  return fb_party_teams_status;
}

std::vector<flatbuffers::Offset<fb::arn::MemberStatus>>
flatbuffer_generator::generate_contender_vec_status_offset(const std::vector<fighting_contender_sptr>& contenders) {
  std::vector<flatbuffers::Offset<fb::arn::MemberStatus>> fb_character_status;

  fb_character_status.reserve(contenders.size());
  for (const auto& contender : contenders) {
    const auto& status = contender->status();
    const auto fbStatus = fb::arn::CharacterStatus{
        contender->id(),
        true,// is_contender
        status.life_pt.current,
        status.life_pt.total,
        status.magic_pt.current,
        status.magic_pt.total,
        util::convert_arena_orient_to_fb(contender->side_orient())};
    fb_character_status.emplace_back(
        fb::arn::CreateMemberStatus(_fbb,
                                    _fbb.CreateString(contender->name()), &fbStatus));
  }
  return fb_character_status;
}

std::vector<flatbuffers::Offset<fb::arn::MemberStatus>>
flatbuffer_generator::generate_team_member_vec_status_offset(const std::vector<team_member_sptr>& members) {
  std::vector<flatbuffers::Offset<fb::arn::MemberStatus>> fb_character_status;

  fb_character_status.reserve(members.size());
  for (const auto& member : members) {
    const auto& status = member->status();
    const auto fbStatus = fb::arn::CharacterStatus{
        member->id(),
        true,// is_contender
        status.life_pt.current,
        status.life_pt.total,
        status.magic_pt.current,
        status.magic_pt.total,
        util::convert_arena_orient_to_fb(member->side_orient())};
    fb_character_status.emplace_back(
        fb::arn::CreateMemberStatus(_fbb,
                                    _fbb.CreateString(member->name()), &fbStatus));
  }
  return fb_character_status;
}

std::vector<flatbuffers::Offset<fb::arn::Reward>>
flatbuffer_generator::generate_rewards_offset(const rewards& rewards) {
  std::vector<flatbuffers::Offset<fb::arn::Reward>> fb_rewards;
  fb_rewards.reserve(rewards.keys.size());
  for (unsigned i = 0; i < rewards.keys.size(); ++i) {
    fb_rewards.emplace_back(fb::arn::CreateReward(_fbb,
                                                  _fbb.CreateString(rewards.keys.at(i)),
                                                  rewards.quantity.at(i)));
  }
  return fb_rewards;
}

//flatbuffers::Offset<fb::arn::Cosmetics>
//FlatbufferGenerator::generateCosmeticsOffset(const PartyTeam&)
//{
//}
//
//flatbuffers::Offset<fb::arn::Cosmetics>
//FlatbufferGenerator::generateCosmeticsOffset(const std::vector<fys::arena::FightingContender>& contenders)
//{
//}

}// namespace fys::arena