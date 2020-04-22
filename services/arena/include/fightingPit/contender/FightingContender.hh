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


#ifndef FYS_FIGHTINGCONTENDER_HH
#define FYS_FIGHTINGCONTENDER_HH

#include <memory>
#include <fightingPit/data/CommonTypes.hh>
#include <fightingPit/FightingPitLayout.hh>

namespace fys::arena {

// forward declarations
class PitContenders;
class ContenderScripting;
class AllyPartyTeams;

class FightingContender {
	friend class FightingPitLayout;

public:
	explicit FightingContender(std::unique_ptr<ContenderScripting> contenderScripting);
	FightingContender(const FightingContender& other) = delete;
	FightingContender(FightingContender&& other) noexcept = default;

	void executeAction();

	[[nodiscard]] bool
	setupContender();

	[[nodiscard]] const data::Status&
	getStatus() const { return _status; }

	[[nodiscard]] const HexagonSide&
	getHexagonSide() const { return _side; }

	[[nodiscard]] data::Status&
	accessStatus() { return _status; }

	[[nodiscard]] HexagonSide::Orientation
	getHexagonSideOrient() const { return (*_side).second; }

	[[nodiscard]] const std::unique_ptr<ContenderScripting>&
	getContenderScripting() const { return _contenderScripting; }

	[[nodiscard]] const std::string&
	getName() const;

	[[nodiscard]] unsigned
	getId() const;

private:
	std::unique_ptr<ContenderScripting> _contenderScripting;
	HexagonSide _side;
	data::Status _status;
};

using FightingContenderSPtr = std::shared_ptr<FightingContender>;

}

#endif // !FYS_FIGHTINGCONTENDER_HH
