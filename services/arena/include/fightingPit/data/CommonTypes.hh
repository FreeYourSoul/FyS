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

struct PriorityElem { // Improve with strong typing on ID/SPEED
	PriorityElem() = default;
	PriorityElem(uint aid, int aspeed, bool aisContender)
			:id(aid), speed(aspeed), isContender(aisContender) { }

	uint id;
	int speed;
	bool isContender;

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

/**
 * Representation of a status alteration of a character
 */
class Alteration {
public:
	struct CompAlteration {
		bool operator()(const Alteration& lhs, const Alteration& rhs) const
		{
			return lhs._alterationKey == rhs._alterationKey && lhs._lvl == rhs._lvl;
		}
	};

	Alteration(std::string alterationKey, uint lvl, uint turn) noexcept
			:_alterationKey(std::move(alterationKey)), _lvl(lvl), _turn(turn) { }

	/**
	 * Process an alteration action, return if it has an impact on the turn of the player
	 * @return false if the player turn is skipped, true otherwise
	 */
	bool processAlteration()
	{
		if (_action && _turn-- > 0) {
			return _action(_lvl, _turn);
		}
		return true;
	}

	const std::string& getAlterationKey() const { return _alterationKey; }
	uint getTurn() const { return _turn; }

private:
	std::string _alterationKey;
	uint _lvl;
	uint _turn;
	std::function<bool(uint, uint)> _action;
};

struct Status {
	Life life;
	MagicPoint magicPoint;
	uint initialSpeed;
	std::set<Alteration, Alteration::CompAlteration> alteration_before;
	std::set<Alteration, Alteration::CompAlteration> alterations;
	std::set<Alteration, Alteration::CompAlteration> alteration_after;
};

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
