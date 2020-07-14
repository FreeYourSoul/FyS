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


#ifndef FYS_ALLYPARTYTEAMS_HH
#define FYS_ALLYPARTYTEAMS_HH

#include <vector>
#include <memory>

// forward declarations
namespace chaiscript {
class ChaiScript;
}
namespace fys::arena {
class party_team;
class team_member;
class pit_contenders;
}

namespace fys::arena {

template<typename T>
using comparator_selection = std::function<bool(std::shared_ptr<T>, std::shared_ptr<T>)>;

class ally_party_teams {

public:
	~ally_party_teams();

	[[nodiscard]] std::shared_ptr<team_member>
	select_suitable_member(comparator_selection<team_member> comp);

	[[nodiscard]] std::shared_ptr<team_member>
	select_suitable_member_on_side(hexagon_side::orientation side, comparator_selection<team_member> comp);

	[[nodiscard]] std::shared_ptr<team_member>
	select_suitable_member_alive(comparator_selection<team_member> comp);

	[[nodiscard]] std::shared_ptr<team_member>
	select_suitable_member_on_side_alive(hexagon_side::orientation side, comparator_selection<team_member> comp);

	[[nodiscard]] std::shared_ptr<team_member>
	select_random_member_on_side_alive(hexagon_side::orientation side);

	[[nodiscard]] std::shared_ptr<team_member>
	select_member_by_id(unsigned id_member);

	[[nodiscard]] std::vector<std::shared_ptr<team_member>>
	members_by_side(hexagon_side::orientation side) const;

	[[nodiscard]] std::vector<std::shared_ptr<team_member>>
	dead_members_by_side(hexagon_side::orientation side) const;

	[[nodiscard]] std::shared_ptr<team_member>
	get_specific_team_member_by_id(const std::string& user_name, unsigned id_member) const;

	[[nodiscard]] unsigned
	number_ally() const { return _current_team_member_id; }

	[[nodiscard]] unsigned
	ally_number_on_side(hexagon_side::orientation side) const;

	[[nodiscard]] const party_team&
	get_party_team_of_player(const std::string& party_team) const;

	[[nodiscard]] bool
	all_dead() const;

	/**
	 * Validate the readiness of a player to start the battle
	 * @param party_team name of the player validating its readiness
	 * @return true if all players are ready, false otherwise
	 */
	[[nodiscard]] bool
	set_party_readiness(const std::string& party_team);

	[[nodiscard]] const std::vector<std::unique_ptr<party_team>>&
	party_teams() const;

	[[nodiscard]] uint
	fighting_pit_id() const { return _fighting_pit_id; }

	/**
	 * Get the next registered action of the given ally, and execute it
	 * @param ally member to execute an action from (if the action queue is filled)
	 * @param pc PitContender used to check if the target of the action is correct
	 * @param chai_ptr chai engine pointer to execute the action
	 * @return true if an action has been executed, false otherwise
	 */
	[[nodiscard]] bool
	execute_ally_action(const data::priority_elem& ally, pit_contenders& pc, std::unique_ptr<chaiscript::ChaiScript>& chai_ptr);

	void add_party_team(std::unique_ptr<party_team>&& team);
	void set_fighting_pit_id(uint fighting_pit_id) { _fighting_pit_id = fighting_pit_id; }

private:
	std::vector<std::unique_ptr<party_team>> _party_teams;

	// automatically incremented for each TeamMember when adding a PartyTeam
	unsigned _current_team_member_id{0};
	unsigned _fighting_pit_id;

};

}

#endif //FYS_ALLYPARTYTEAMS_HH
