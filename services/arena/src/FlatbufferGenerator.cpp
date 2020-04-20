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

#include <fightingPit/FightingPit.hh>
#include <fightingPit/contender/FightingContender.hh>
#include <fightingPit/contender/ContenderScripting.hh>
#include <fightingPit/team/AllyPartyTeams.hh>
#include <fightingPit/team/PartyTeam.hh>
#include <fightingPit/team/TeamMember.hh>

#include <FbUtility.hh>

#include "FlatbufferGenerator.hh"

namespace {

[[nodiscard]] std::vector<std::string>
retrieveStringVector(const std::vector<std::pair<std::string, uint>> doableAttacks)
{
	std::vector<std::string> stringVec;
	stringVec.reserve(doableAttacks.size());
	for (const auto&[atkString, atkLvl]: doableAttacks) {
		stringVec.emplace_back(atkString);
	}
	return stringVec;
}

}

namespace fys::arena {

std::pair<void*, uint>
FlatbufferGenerator::generateFightingPitState(const fys::arena::FightingPit& fp)
{
	_fbb.Clear();
	auto fbPartyTeamVec = _fbb.CreateVector(generatePartyTeamVecStatusOffset(fp.getPartyTeams()));
	auto fbContenderVec = _fbb.CreateVector(generateContenderVecStatusOffset(fp.getPitContenders()));
	auto fps = fb::CreateFightingPitState(
			_fbb,
			fp.getId(),
			fbPartyTeamVec,
			fbContenderVec
	);
	fb::FinishFightingPitStateBuffer(_fbb, fps);
	return std::pair(_fbb.GetBufferPointer(), _fbb.GetSize());
}

std::pair<void*, uint>
FlatbufferGenerator::generatePartyTeamStatus(const PartyTeam& partyTeam)
{
	_fbb.Clear();
	auto fbName = _fbb.CreateString(partyTeam.getUserName());
	auto fbMemberVec = _fbb.CreateVector(generateTeamMemberVecStatusOffset(partyTeam));
	auto fbVecAttacks = [this](const PartyTeam& partyTeam) {
		std::vector<std::string> vecString;
		for (const auto& tm : partyTeam.getTeamMembers()) {
			auto vec = retrieveStringVector(tm->getActionsDoable());
			std::move(vec.begin(), vec.end(), std::back_inserter(vecString));
		}
		return _fbb.CreateVectorOfStrings(vecString);
	}(partyTeam);

	fb::PartyTeamStatusBuilder builder(_fbb);
	builder.add_user_name(fbName);
	builder.add_members(fbMemberVec);
	builder.add_attacks(fbVecAttacks);
	_fbb.Finish(builder.Finish());

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

std::vector<flatbuffers::Offset<fb::PartyTeamStatus>>
FlatbufferGenerator::generatePartyTeamVecStatusOffset(const AllyPartyTeams& apt)
{
	std::vector<flatbuffers::Offset<fb::PartyTeamStatus>> fbPartyTeamsStatus;
	const auto& partyTeams = apt.getPartyTeams();

	fbPartyTeamsStatus.reserve(partyTeams.size());
	for (const auto& pt : partyTeams) {
		auto fbUserName = _fbb.CreateString(pt->getUserName());
		auto fbMembers = _fbb.CreateVector(generateTeamMemberVecStatusOffset(*pt));
		auto fbAttacks = [this](const auto& pt) {
			std::vector<std::string> attacks;
			for (const auto& tm : pt->getTeamMembers()) {
				auto vec = retrieveStringVector(tm->getActionsDoable());
				std::move(vec.begin(), vec.end(), std::back_inserter(attacks));
			}
			return _fbb.CreateVectorOfStrings(attacks);
		}(pt);

		fb::PartyTeamStatusBuilder builder(_fbb);
		builder.add_user_name(fbUserName);
		builder.add_members(fbMembers);
		builder.add_attacks(fbAttacks);
		fbPartyTeamsStatus.emplace_back(builder.Finish());
	}
	return fbPartyTeamsStatus;
}

std::vector<flatbuffers::Offset<fb::MemberStatus>>
FlatbufferGenerator::generateContenderVecStatusOffset(const PitContenders& pitContender)
{
	std::vector<flatbuffers::Offset<fb::MemberStatus>> fbCharacterStatus;
	const auto& contenders = pitContender.getContenders();

	fbCharacterStatus.reserve(contenders.size());
	for (const auto& contender : contenders) {
		const auto& status = contender->getStatus();
		const auto fbStatus = fb::CharacterStatus{
				contender->getContenderScripting()->getContenderId(),
				true, // is_contender
				status.life.current,
				status.life.total,
				status.magicPoint.current,
				status.magicPoint.total,
				util::convertArenaOrientToFb(contender->getHexagonSideOrient())
		};
		fbCharacterStatus.emplace_back(
				fb::CreateMemberStatus(_fbb,
						_fbb.CreateString(contender->getContenderScripting()->getContenderName()), &fbStatus));
	}
	return fbCharacterStatus;
}

std::vector<flatbuffers::Offset<fb::MemberStatus>>
FlatbufferGenerator::generateTeamMemberVecStatusOffset(const PartyTeam& partyTeam)
{
	std::vector<flatbuffers::Offset<fb::MemberStatus>> fbCharacterStatus;
	const auto& members = partyTeam.getTeamMembers();

	fbCharacterStatus.reserve(members.size());
	for (const auto& member : members) {
		const auto& status = member->getStatus();
		const auto fbStatus = fb::CharacterStatus{
				member->getId(),
				true, // is_contender
				status.life.current,
				status.life.total,
				status.magicPoint.current,
				status.magicPoint.total,
				util::convertArenaOrientToFb(member->getHexagonSideOrient())
		};
		fbCharacterStatus.emplace_back(
				fb::CreateMemberStatus(_fbb,
						_fbb.CreateString(member->getName()), &fbStatus));
	}
	return fbCharacterStatus;
}

//flatbuffers::Offset<fb::Cosmetics>
//FlatbufferGenerator::generateCosmeticsOffset(const PartyTeam&)
//{
//}
//
//flatbuffers::Offset<fb::Cosmetics>
//FlatbufferGenerator::generateCosmeticsOffset(const std::vector<fys::arena::FightingContender>& contenders)
//{
//}

}
