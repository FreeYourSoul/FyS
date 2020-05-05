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


#ifndef FYS_COMMONDATA_HH
#define FYS_COMMONDATA_HH

#include <vector>
#include <set>
#include <functional>
#include <fightingPit/HexagonSide.hh>

//! Types and functions linking C++ world with ChaiScript
namespace fys::arena::data {
static constexpr bool CONTENDER = true;
static constexpr bool PARTY_MEMBER = false;

[[nodiscard]] inline std::string
getActionNameFromKey(const std::string& key)
{
	auto startSeparator = key.find_last_of(':');
	auto endSeparator = key.find_last_of('.');

	if (startSeparator != std::string::npos && endSeparator != std::string::npos) {
		// if a ':' and a '.' are found
		return key.substr(startSeparator + 1, endSeparator - startSeparator - 1);
	}
	else if (startSeparator == std::string::npos && endSeparator != std::string::npos) {
		// if a ':' is not found but not a '.' is found
		return key.substr(0, endSeparator);
	}
	else if (startSeparator != std::string::npos && endSeparator == std::string::npos) {
		// if a ':' is found but not a '.' is not found
		return key.substr(startSeparator + 1);
	}
	return key;
}

[[nodiscard]] inline std::string
getAlterationNameFromKey(const std::string& key)
{
	std::string prefix = "alteration_";
	return prefix.append(getActionNameFromKey(key));
}

struct PriorityElem { // Improve with strong typing on ID/SPEED
	PriorityElem() = default;
	PriorityElem(uint aid, int aspeed, bool aisContender)
			:id(aid), speed(aspeed), isContender(aisContender) { }

	uint id;
	int speed;
	bool isContender = false;

	[[nodiscard]] PriorityElem&
	operator-(const PriorityElem& other)
	{
		speed -= other.speed;
		return *this;
	}

	[[nodiscard]] bool
	operator<(const PriorityElem& other) const
	{
		return speed < other.speed;
	}

	[[nodiscard]] bool
	operator==(const PriorityElem& other) const
	{
		return id == other.id && isContender == other.isContender;
	}

};

struct Life {
	uint current = 0;
	uint total = 0;

	[[nodiscard]] bool
	isDead() const { return current == 0; }
};

struct MagicPoint {
	uint current = 0;
	uint total = 0;
};

struct Status;

/**
 * Representation of a status alteration of a character
 */
class Alteration {
public:
	Alteration(std::string alterationKey, uint lvl, uint turn, std::function<int(data::Status&, uint, uint)> a) noexcept
			:
			_alterationKey(std::move(alterationKey)),
			_lvl(lvl),
			_turn(turn),
			_action(std::move(a)) { }

	/**
	 * Process an alteration action, return if it has an impact on the turn of the player
	 * @return false if the player turn is skipped, true otherwise
	 */
	bool processAlteration(Status& status)
	{
		if (_action && _turn > 0) {
			--_turn;
			return _action(status, _lvl, _turn);
		}
		return true;
	}

	const std::string& getAlterationKey() const { return _alterationKey; }
	uint getTurn() const { return _turn; }

private:
	std::string _alterationKey;
	uint _lvl;
	uint _turn;
	std::function<int(Status&, uint, uint)> _action;
};

struct Status {
	Life life;
	MagicPoint magicPoint;
	uint initialSpeed;

	/**
	 * When moving the appropriate flag is set in FightingPitLayout which use
	 * this member to set the new position of the character
	 */
	HexagonSide::Orientation moving = HexagonSide::Orientation::NONE;

	std::vector<Alteration> alteration_before;
	std::vector<Alteration> alteration_after;

	//! Alteration executed on the turn (usually stat modification impacting the damages/heal)
	std::vector<Alteration> alterations;

	/**
	 * Process all alteration before turn stored
	 * @return true if the turn is executed, false if it is cancelled
	 */
	bool processAlterationBeforeTurn()
	{
		bool processTurn = true;
		for (auto& alteration : alteration_before) {
			processTurn &= alteration.processAlteration(*this);
		}
		return processTurn;
	}

	void processAlterationAfterTurn()
	{
		for (auto& alteration : alteration_after) {
			alteration.processAlteration(*this);
		}
	}

	void cleanupFinishedAlteration()
	{
		alterations.erase(std::remove_if(alterations.begin(), alterations.end(),
				[](const auto& alt) { return alt.getTurn() <= 0; }), alterations.end());
		alteration_before.erase(std::remove_if(alteration_before.begin(), alteration_before.end(),
				[](const auto& alt) { return alt.getTurn() <= 0; }), alteration_before.end());
		alteration_after.erase(std::remove_if(alteration_after.begin(), alteration_after.end(),
				[](const auto& alt) { return alt.getTurn() <= 0; }), alteration_after.end());
	}

};

/**
 * Merge the alterations, remove doubles, but keep the alteration that would last the longest.
 * @param toModify reference on the alterations vector to modify
 * @param toMerge additional alternations
 * @param replaceIfExist replace the alteration if it is already existing if set at true,
 * do not add the alteration otherwise
 */
static void
mergeAlterations(std::vector<Alteration>& toModify, std::vector<Alteration> toMerge, bool replaceIfExist = false)
{
	(void)mergeAlterations; // suppress unused warning (as it is used, but by ChaiScript)

	std::move(toMerge.begin(), toMerge.end(), std::back_inserter(toModify));
	std::sort(toModify.begin(), toModify.end(), [replaceIfExist](const Alteration& lhs, const Alteration& rhs) {
		return lhs.getAlterationKey() == rhs.getAlterationKey() &&
				((replaceIfExist && lhs.getTurn() > rhs.getTurn()) || (!replaceIfExist && lhs.getTurn() < rhs.getTurn()));
	});
	toModify.erase(std::unique(toModify.begin(), toModify.end(), [](const Alteration& lhs, const Alteration& rhs) {
		return lhs.getAlterationKey() == rhs.getAlterationKey();
	}), toModify.end());
}

enum MoveDirection {
	BACK,
	RIGHT,
	LEFT
};

enum Targeting {
	SELF = 0,
	ENNEMY = 1,
	ALLY = 2,
	ENNEMIES = 3,
	ALLIES = 4,
	ALLY_AND_ENNEMY = 5,
	ALLY_OR_ENNEMY = 6,
	SIDE = 7
};

} // namespace fys::arena::data


#endif // !FYS_COMMONDATA_HH
