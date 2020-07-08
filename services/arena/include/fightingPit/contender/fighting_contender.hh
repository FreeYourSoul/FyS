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
#include <fightingPit/data/common_types.hh>
#include <fightingPit/fighting_pit_layout.hh>

namespace fys::arena {

// forward declarations
class pit_contenders;
class contender_scripting;
class ally_party_teams;

class fighting_contender {
	friend class fighting_pit_layout;

public:
	explicit fighting_contender(std::unique_ptr<contender_scripting> contender_scripting);
	fighting_contender(const fighting_contender& other) = delete;
	fighting_contender(fighting_contender&& other) noexcept = default;

	void execute_action();

	[[nodiscard]] bool
	setup_contender();

	[[nodiscard]] const data::status&
	get_status() const { return _status; }

	[[nodiscard]] const hexagon_side&
	get_hexagon_side() const { return _side; }

	[[nodiscard]] data::status&
	access_status() { return _status; }

	[[nodiscard]] hexagon_side::orientation
	get_hexagon_side_orient() const { return (*_side).second; }

	[[nodiscard]] const std::unique_ptr<contender_scripting>&
	get_contender_scripting() const { return _contender_scripting; }

	[[nodiscard]] const std::string&
	get_name() const;

	[[nodiscard]] unsigned
	get_id() const;

private:
	std::unique_ptr<contender_scripting> _contender_scripting;
	hexagon_side _side;
	data::status _status;

	/**
	 * When moving the appropriate flag is set in FightingPitLayout which use
	 * this member to set the new position of the character
	 */
	hexagon_side::orientation _moving = hexagon_side::orientation::NONE;
};

using fighting_contender_sptr = std::shared_ptr<fighting_contender>;

}

#endif // !FYS_FIGHTINGCONTENDER_HH
