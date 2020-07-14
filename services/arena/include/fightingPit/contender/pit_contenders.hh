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


#ifndef FYS_PITCONTENDERS_HH
#define FYS_PITCONTENDERS_HH

#include <memory>
#include <vector>
#include <functional>

#include <fightingPit/data/common_types.hh>
#include <fightingPit/hexagon_side.hh>

namespace fys::arena {

template<typename T>
using comparator_selection = std::function<bool(std::shared_ptr<T>, std::shared_ptr<T>)>;

//forward declarations
class fighting_contender;
class ally_party_teams;

class pit_contenders {

public:
	pit_contenders() = default;
	pit_contenders(const pit_contenders& other) = delete;

	void execute_contender_action(const data::priority_elem& contender);


	[[nodiscard]] std::vector<std::shared_ptr<fighting_contender>>
	contenders_on_side(hexagon_side::orientation side) const;

	[[nodiscard]] std::vector<std::shared_ptr<fighting_contender>>
	changing_side_contenders() const;

	// scripting utility
	[[nodiscard]] std::shared_ptr<fighting_contender>
	select_suitable_contender(comparator_selection<fighting_contender> comp) const;

	[[nodiscard]] std::shared_ptr<fighting_contender>
	select_suitable_contender_on_side(hexagon_side::orientation side, comparator_selection<fighting_contender> comp) const;

	[[nodiscard]] std::shared_ptr<fighting_contender>
	select_random_contender_on_side_alive(hexagon_side::orientation side) const;

	[[nodiscard]] std::shared_ptr<fighting_contender>
	select_suitable_contender_alive(comparator_selection<fighting_contender> comp) const;

	[[nodiscard]] std::shared_ptr<fighting_contender>
	select_suitable_contender_on_side_alive(hexagon_side::orientation side, comparator_selection<fighting_contender> comp) const;

	[[nodiscard]] std::shared_ptr<fighting_contender>
	fighting_contender_at(uint pos) const { return _contenders.at(pos); }

	[[nodiscard]] std::size_t
	number_contender() const { return _contenders.size(); }

	[[nodiscard]] std::vector<std::shared_ptr<fighting_contender>>
	get_dead_contender_on_side(hexagon_side::orientation contender_ptr) const;

	[[nodiscard]] const std::vector<std::shared_ptr<fighting_contender> >&
	contenders() const { return _contenders; }

	[[nodiscard]] unsigned
	number_contender_on_side(hexagon_side::orientation side) const;

	[[nodiscard]] bool
	add_contender(const std::shared_ptr<fighting_contender>& contender);

	[[nodiscard]] bool
	all_dead() const;

private:
	std::vector<std::shared_ptr<fighting_contender>> _contenders;

	/**
	 * Flags determining which contenders are going to move from one side to another
	 * (only _contenders having this flag (index equivalent) set to true have their position refreshed
	 */
	std::vector<bool> _change_side_flags;

};

}

#endif // !FYS_PITCONTENDERS_HH
