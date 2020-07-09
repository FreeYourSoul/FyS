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

#include <catch2/catch.hpp>

#define private protected
#include <fightingPit/priority_order_list.hh>
#undef private

// some data are not checked during the comparison and thus ignored (0 set instead of an explicit value that won't be checked anyway)
constexpr static std::uint32_t DONT_CARE = 0;

class PriorityOrderListPassThrough : public fys::arena::priority_order_list {
public:
	void
	sort_base_and_calculate_priority()
	{
		_priorityList.clear();
		fys::arena::priority_order_list::sort_base_and_calculate_priority();
	}

	using priority_order_list::priority_order_list;
	using priority_order_list::_priorityList;
	using priority_order_list::_analyzed_list;
	using priority_order_list::_base_speed;
};

TEST_CASE("PriorityOrderListTestCase priorityList test", "[service][arena]")
{
	std::vector<fys::arena::data::priority_elem> baseSpeed = {
			// id, speed, isContender
			{1, 10, fys::arena::data::CONTENDER},
			{2, 19, fys::arena::data::PARTY_MEMBER},
			{3, 3, fys::arena::data::PARTY_MEMBER},
			{4, 2, fys::arena::data::CONTENDER},
			{5, 30, fys::arena::data::CONTENDER}
	};
	PriorityOrderListPassThrough pol(baseSpeed);

	SECTION("Check instantiation test") {

		REQUIRE(5 == pol._priorityList.size());
		REQUIRE(5 == pol._analyzed_list.size());
		REQUIRE(fys::arena::data::priority_elem{5, 30, fys::arena::data::CONTENDER} == pol._base_speed.at(4));
		REQUIRE(fys::arena::data::priority_elem(2, 19, fys::arena::data::PARTY_MEMBER) == pol._base_speed.at(3));
		REQUIRE(fys::arena::data::priority_elem(1, 10, fys::arena::data::CONTENDER) == pol._base_speed.at(2));
		REQUIRE(fys::arena::data::priority_elem(3, 3, fys::arena::data::PARTY_MEMBER) == pol._base_speed.at(1));
		REQUIRE(fys::arena::data::priority_elem(4, 2, fys::arena::data::CONTENDER) == pol._base_speed.at(0));
		for (std::size_t i = 0; i < pol._base_speed.size(); ++i) {
			REQUIRE(pol._base_speed.at(i) == pol._priorityList.at(i));
			REQUIRE(pol._base_speed.at(i) == pol._analyzed_list.at(i));
		}

	} // End section : Check instantiation test

	SECTION("SortAndCalculatePriority normal setup test") {

		// Base Speed order 5>2>1>3>4
		// 5:30
		// 2:19
		// 1:10
		// 3:3
		// 4:2
		pol.sort_base_and_calculate_priority(); // first turn is following base speed order

		REQUIRE(fys::arena::data::priority_elem(5, 30, fys::arena::data::CONTENDER) == pol._analyzed_list.at(4));
		REQUIRE(fys::arena::data::priority_elem(2, 19, fys::arena::data::PARTY_MEMBER) == pol._analyzed_list.at(3));
		REQUIRE(fys::arena::data::priority_elem(1, 10, fys::arena::data::CONTENDER) == pol._analyzed_list.at(2));
		REQUIRE(fys::arena::data::priority_elem(3, 3, fys::arena::data::PARTY_MEMBER) == pol._analyzed_list.at(1));
		REQUIRE(fys::arena::data::priority_elem(4, 2, fys::arena::data::CONTENDER) == pol._analyzed_list.at(0));

		SECTION("Turn one check (init)") {

			pol.sort_base_and_calculate_priority();

			/**
			5:30  -->  30 - 19  --> 5:11     Added 5
			2:19  ----------------> 2:19
			1:10  ----------------> 1:10
			3:3   ----------------> 3:3
			4:2   ----------------> 4:2

			2:19 -->  19 - 10   --> 2:9      Added 2
			5:11 -----------------> 5:11
			1:10 -----------------> 1:10
			3:3  -----------------> 3:3
			4:2  -----------------> 4:2

			5:11 -->  11 - 9    --> 5:2      Added 5
			1:10 -----------------> 1:10
			2:9  -----------------> 2:9
			3:3  -----------------> 3:3
			4:2  -----------------> 4:2

			1:10 -->  10 - 3    --> 1:7      Added 1
			2:9  -----------------> 2:9
			3:3  -----------------> 3:3
			5:2  -----------------> 5:2
			4:2  -----------------> 4:2

			2:9  -->  9 - 7     --> 2:2      Added 2
			1:7  -----------------> 1:7
			3:3  -----------------> 3:3
			5:2  -----------------> 5:2
			4:2  -----------------> 4:2

			1:7  -->  7 - 3     --> 1:4      Added 1
			3:3  -----------------> 3:3
			5:2  -----------------> 5:2
			2:2  -----------------> 2:2
			4:2  -----------------> 4:2

			1:4  -->  4 - 3     --> 1:1      Added 1
			3:3  -----------------> 3:3
			5:2  -----------------> 5:2
			2:2  -----------------> 2:2
			4:2  -----------------> 4:2

			3:3  -->  3 - 2     --> 3:1      Added 3
			5:2  -----------------> 5:2
			2:2  -----------------> 2:2
			4:2  -----------------> 4:2
			1:1  -----------------> 1:1

			5:2  -->  2 - 2     --> 5:0      Added 5
			2:2  -----------------> 2:2
			4:2  -----------------> 4:2
			1:1  -----------------> 1:1
			3:1  -----------------> 3:1

			2:2  -->  2 - 1     --> 2:1      Added 2
			4:2  -----------------> 4:2
			1:1  -----------------> 1:1
			3:1  -----------------> 3:1
			5:0  -----------------> 5:0
									.___.
			4:2  -->  2 - 0     --> |4:2|      Added 4 End of turn
			2:1  -----------------> |2:1|
			1:1  -----------------> |1:1|
			3:1  -----------------> |3:1|
			5:0  -----------------> |5:0|
									'¯¯¯'
			*/

			REQUIRE(11 == pol._priorityList.size());
			REQUIRE(5 == pol._priorityList.at(10).id);
			REQUIRE(2 == pol._priorityList.at(9).id);
			REQUIRE(5 == pol._priorityList.at(8).id);
			REQUIRE(1 == pol._priorityList.at(7).id);
			REQUIRE(2 == pol._priorityList.at(6).id);
			REQUIRE(1 == pol._priorityList.at(5).id);
			REQUIRE(1 == pol._priorityList.at(4).id);
			REQUIRE(3 == pol._priorityList.at(3).id);
			REQUIRE(5 == pol._priorityList.at(2).id);
			REQUIRE(2 == pol._priorityList.at(1).id);
			REQUIRE(4 == pol._priorityList.at(0).id);

			//   END OF TURN CALCULATION
			//      REST + BASE + MAX
			//
			// 4:2  -->  2+2 +30    --> 4:34
			// 2:1  -->  1+19+30    --> 2:50
			// 1:1  -->  1+10+30    --> 1:41
			// 3:1  -->  1+3 +30     --> 3:34
			// 5:0  -->  0+30+30    --> 5:60
			// ordered
			// 5:60
			// 2:50
			// 1:41
			// 3:34
			// 4:34
			REQUIRE(fys::arena::data::priority_elem(5, 60, fys::arena::data::CONTENDER) == pol._analyzed_list.at(4));
			REQUIRE(fys::arena::data::priority_elem(2, 50, fys::arena::data::PARTY_MEMBER) == pol._analyzed_list.at(3));
			REQUIRE(fys::arena::data::priority_elem(1, 41, fys::arena::data::CONTENDER) == pol._analyzed_list.at(2));
			REQUIRE(fys::arena::data::priority_elem(3, 34, fys::arena::data::PARTY_MEMBER) == pol._analyzed_list.at(1));
			REQUIRE(fys::arena::data::priority_elem(4, 34, fys::arena::data::CONTENDER) == pol._analyzed_list.at(0));

		} // End section : Turn one check (init)

		SECTION("Turn two check") {
			pol.sort_base_and_calculate_priority();
			pol.sort_base_and_calculate_priority();
			/**
			5:60  -->  60 - 50  --> 5:10     Added 5
			2:50  ----------------> 2:50
			1:41  ----------------> 1:41
			3:34  ----------------> 3:34
			4:34  ----------------> 4:34

			2:50  -->  50 - 41  --> 2:9      Added 2
			1:41  ----------------> 1:41
			3:34  ----------------> 3:34
			4:34  ----------------> 4:34
			5:10  ----------------> 5:10

			1:41  -->  41 - 34  --> 1:7      Added 1
			3:34  ----------------> 3:34
			4:34  ----------------> 4:34
			5:10  ----------------> 5:10
			2:9   ----------------> 2:9

			3:34  -->  34 - 34  --> 3:0      Added 3
			4:34  ----------------> 4:34
			5:10  ----------------> 5:10
			2:9   ----------------> 2:9
			1:7   ----------------> 1:7
									.____.
			4:34  -->  34 - 10  --> |4:24|     Added 4 End of turn
			5:10  ----------------> |5:10|
			2:9   ----------------> |2:9 |
			1:7   ----------------> |1:7 |
			3:0   ----------------> |3:0 |
									'¯¯¯¯'
			*/
			REQUIRE(5 == pol._priorityList.size());

			REQUIRE(5 == pol._priorityList.at(4).id);
			REQUIRE(2 == pol._priorityList.at(3).id);
			REQUIRE(1 == pol._priorityList.at(2).id);
			REQUIRE(3 == pol._priorityList.at(1).id);
			REQUIRE(4 == pol._priorityList.at(0).id);

			//   END OF TURN CALCULATION
			//      REST + BASE + MAX
			//
			// 4:24  --> 24+2 +30    --> 4:56
			// 2:9  -->  9 +19+30    --> 2:58
			// 1:7  -->  7 +10+30    --> 1:47
			// 3:0  -->  0 +3 +30    --> 3:33
			// 5:10 -->  10+30+30    --> 5:70
			// ordered
			// 5:70
			// 2:58
			// 4:56
			// 1:47
			// 3:33
			REQUIRE(fys::arena::data::priority_elem(5, 70, fys::arena::data::CONTENDER) == pol._analyzed_list.at(4));
			REQUIRE(fys::arena::data::priority_elem(2, 58, fys::arena::data::PARTY_MEMBER) == pol._analyzed_list.at(3));
			REQUIRE(fys::arena::data::priority_elem(4, 56, fys::arena::data::CONTENDER) == pol._analyzed_list.at(2));
			REQUIRE(fys::arena::data::priority_elem(1, 47, fys::arena::data::CONTENDER) == pol._analyzed_list.at(1));
			REQUIRE(fys::arena::data::priority_elem(3, 33, fys::arena::data::PARTY_MEMBER) == pol._analyzed_list.at(0));

		} // End section : Turn two check

		SECTION("Turn three check") {
			pol.sort_base_and_calculate_priority();
			pol.sort_base_and_calculate_priority();
			pol.sort_base_and_calculate_priority();

			/**
			5:70  -->  70 - 58  --> 5:12     Added 5
			2:58  ----------------> 2:58
			4:56  ----------------> 4:56
			1:47  ----------------> 1:47
			3:33  ----------------> 3:33

			2:58  -->  58 - 47  --> 2:11     Added 2
			4:56  ----------------> 4:56
			1:47  ----------------> 1:47
			3:33  ----------------> 3:33
			5:12  ----------------> 5:12
									.____.
			4:56  -->  56 - 12  --> |4:44|      Added 4 End of turn
			1:47  ----------------> |1:47|
			3:33  ----------------> |3:33|
			5:12  ----------------> |5:12|
			2:11  ----------------> |2:11|
									'¯¯¯¯'
			*/
			REQUIRE(3 == pol._priorityList.size());

			REQUIRE(5 == pol._priorityList.at(2).id);
			REQUIRE(2 == pol._priorityList.at(1).id);
			REQUIRE(4 == pol._priorityList.at(0).id);

			//   END OF TURN CALCULATION
			//      REST + BASE + MAX
			//
			// 4:44  -->  44+2 +30    --> 4:76
			// 2:11  -->  11+19+30    --> 2:60
			// 1:47  -->  47+10+30    --> 1:87
			// 3:33  -->  33+3 +30    --> 3:66
			// 5:12  -->  12+30+30    --> 5:72
			// ordered
			// 1:87
			// 5:76
			// 3:72
			// 2:66
			// 4:60
			REQUIRE(fys::arena::data::priority_elem(1, 87, fys::arena::data::CONTENDER) == pol._analyzed_list.at(4));
			REQUIRE(fys::arena::data::priority_elem(4, 76, fys::arena::data::CONTENDER) == pol._analyzed_list.at(3));
			REQUIRE(fys::arena::data::priority_elem(5, 72, fys::arena::data::CONTENDER) == pol._analyzed_list.at(2));
			REQUIRE(fys::arena::data::priority_elem(3, 66, fys::arena::data::PARTY_MEMBER) == pol._analyzed_list.at(1));
			REQUIRE(fys::arena::data::priority_elem(2, 60, fys::arena::data::PARTY_MEMBER) == pol._analyzed_list.at(0));

		} // End section : Turn three check


	} // End section : SortAndCalculatePriority normal setup test

	SECTION("Edge cases SortAndCalculatePriority test") {

		SECTION("Empty side") {
			std::vector<fys::arena::data::priority_elem> baseSpeed_empty = {};
			PriorityOrderListPassThrough polEmpty(baseSpeed_empty);

			REQUIRE(polEmpty._priorityList.empty());
			REQUIRE(polEmpty._analyzed_list.empty());
			REQUIRE(polEmpty._base_speed.empty());
			REQUIRE(polEmpty.empty());

			polEmpty.sort_base_and_calculate_priority();
			REQUIRE(polEmpty._priorityList.empty());
			REQUIRE(polEmpty._analyzed_list.empty());
			REQUIRE(polEmpty._base_speed.empty());
			REQUIRE(polEmpty.empty());

			REQUIRE(fys::arena::data::priority_elem(0, 0, fys::arena::data::PARTY_MEMBER) == polEmpty.next());

			polEmpty.rm_participant_from_list(1, false);
			polEmpty.rm_participant_from_list(2, true);
			polEmpty.rm_participant_from_list(3, false);
			REQUIRE(polEmpty._priorityList.empty());
			REQUIRE(polEmpty._analyzed_list.empty());
			REQUIRE(polEmpty._base_speed.empty());
			REQUIRE(polEmpty.empty());
		}

		SECTION("One in side") {
			std::vector<fys::arena::data::priority_elem> baseSpeed_one = {
					{1337, 42, fys::arena::data::PARTY_MEMBER}
			};
			PriorityOrderListPassThrough polOne(baseSpeed_one);

			REQUIRE(1 == polOne._priorityList.size());
			REQUIRE(1 == polOne._analyzed_list.size());
			REQUIRE(1 == polOne._base_speed.size());

			polOne.sort_base_and_calculate_priority();
			REQUIRE(1 == polOne._priorityList.size());
			REQUIRE(1 == polOne._analyzed_list.size());
			REQUIRE(1 == polOne._base_speed.size());

			polOne.rm_participant_from_list(1, fys::arena::data::PARTY_MEMBER);
			polOne.rm_participant_from_list(2, fys::arena::data::PARTY_MEMBER);
			polOne.rm_participant_from_list(3, fys::arena::data::PARTY_MEMBER);
			REQUIRE(1 == polOne._priorityList.size());
			REQUIRE(1 == polOne._analyzed_list.size());
			REQUIRE(1 == polOne._base_speed.size());

			REQUIRE(1337 == polOne.next().id);
			REQUIRE(1337 == polOne.next().id);
			REQUIRE(1337 == polOne.next().id);

		}

	} // End section : Edge cases SortAndCalculatePriority test

	SECTION("Public API test") {

		REQUIRE(5 == pol._priorityList.size());
		REQUIRE(fys::arena::data::priority_elem(5, 30, fys::arena::data::CONTENDER) == pol._base_speed.at(4));
		REQUIRE(fys::arena::data::priority_elem(2, 19, fys::arena::data::PARTY_MEMBER) == pol._base_speed.at(3));
		REQUIRE(fys::arena::data::priority_elem(1, 10, fys::arena::data::CONTENDER) == pol._base_speed.at(2));
		REQUIRE(fys::arena::data::priority_elem(3, 3, fys::arena::data::PARTY_MEMBER) == pol._base_speed.at(1));
		REQUIRE(fys::arena::data::priority_elem(4, 1, fys::arena::data::CONTENDER) == pol._base_speed.at(0));

		SECTION("getNext test") {

			REQUIRE(5 == pol._priorityList.size());
			REQUIRE(fys::arena::data::priority_elem(5, 30, fys::arena::data::CONTENDER) == pol.next());
			REQUIRE(fys::arena::data::priority_elem(2, 19, fys::arena::data::PARTY_MEMBER) == pol.next());
			REQUIRE(fys::arena::data::priority_elem(1, 10, fys::arena::data::CONTENDER) == pol.next());
			REQUIRE(fys::arena::data::priority_elem(3, 3, fys::arena::data::PARTY_MEMBER) == pol.next());
			REQUIRE(fys::arena::data::priority_elem(4, 1, fys::arena::data::CONTENDER) == pol.next());
			REQUIRE(pol._priorityList.empty());

			//turn 1
			REQUIRE(fys::arena::data::priority_elem(5, DONT_CARE, fys::arena::data::CONTENDER) == pol.next());
			REQUIRE(10 == pol._priorityList.size());
			REQUIRE(fys::arena::data::priority_elem(2, DONT_CARE, fys::arena::data::PARTY_MEMBER) == pol.next());
			REQUIRE(fys::arena::data::priority_elem(5, DONT_CARE, fys::arena::data::CONTENDER) == pol.next());
			REQUIRE(fys::arena::data::priority_elem(1, DONT_CARE, fys::arena::data::CONTENDER) == pol.next());
			REQUIRE(fys::arena::data::priority_elem(2, DONT_CARE, fys::arena::data::PARTY_MEMBER) == pol.next());
			REQUIRE(fys::arena::data::priority_elem(1, DONT_CARE, fys::arena::data::CONTENDER) == pol.next());
			REQUIRE(fys::arena::data::priority_elem(1, DONT_CARE, fys::arena::data::CONTENDER) == pol.next());
			REQUIRE(fys::arena::data::priority_elem(3, DONT_CARE, fys::arena::data::PARTY_MEMBER) == pol.next());
			REQUIRE(fys::arena::data::priority_elem(5, DONT_CARE, fys::arena::data::CONTENDER) == pol.next());
			REQUIRE(fys::arena::data::priority_elem(2, DONT_CARE, fys::arena::data::PARTY_MEMBER) == pol.next());
			REQUIRE(fys::arena::data::priority_elem(4, DONT_CARE, fys::arena::data::CONTENDER) == pol.next());
			REQUIRE(pol._priorityList.empty());

			//turn 2
			REQUIRE(fys::arena::data::priority_elem(5, DONT_CARE, fys::arena::data::CONTENDER) == pol.next());
			REQUIRE(4 == pol._priorityList.size());
			REQUIRE(fys::arena::data::priority_elem(2, DONT_CARE, fys::arena::data::PARTY_MEMBER) == pol.next());
			REQUIRE(fys::arena::data::priority_elem(1, DONT_CARE, fys::arena::data::CONTENDER) == pol.next());
			REQUIRE(fys::arena::data::priority_elem(3, DONT_CARE, fys::arena::data::PARTY_MEMBER) == pol.next());
			REQUIRE(fys::arena::data::priority_elem(4, DONT_CARE, fys::arena::data::CONTENDER) == pol.next());
			REQUIRE(pol._priorityList.empty());

			//turn 3
			REQUIRE(fys::arena::data::priority_elem(5, DONT_CARE, fys::arena::data::CONTENDER) == pol.next());
			REQUIRE(2 == pol._priorityList.size());
			REQUIRE(fys::arena::data::priority_elem(2, DONT_CARE, fys::arena::data::PARTY_MEMBER) == pol.next());
			REQUIRE(fys::arena::data::priority_elem(4, DONT_CARE, fys::arena::data::CONTENDER) == pol.next());
			REQUIRE(pol._priorityList.empty());

		} // getNext test

		SECTION("addParticipantInList test") {

			// Check "SortAndCalculatePriority normal setup test"::"Turn one check (init)" to verify that the turn is supposed to be set like that
			REQUIRE(5 == pol._priorityList.size());
			REQUIRE(fys::arena::data::priority_elem{5, 30, fys::arena::data::CONTENDER} == pol._base_speed.at(4));
			REQUIRE(fys::arena::data::priority_elem(2, 19, fys::arena::data::PARTY_MEMBER) == pol._base_speed.at(3));
			REQUIRE(fys::arena::data::priority_elem(1, 10, fys::arena::data::CONTENDER) == pol._base_speed.at(2));
			REQUIRE(fys::arena::data::priority_elem(3, 3, fys::arena::data::PARTY_MEMBER) == pol._base_speed.at(1));
			REQUIRE(fys::arena::data::priority_elem(4, 2, fys::arena::data::CONTENDER) == pol._base_speed.at(0));

			pol.add_participant_in_list(6, 15, fys::arena::data::PARTY_MEMBER);

			REQUIRE(6 == pol._base_speed.size());
			REQUIRE(fys::arena::data::priority_elem{5, 30, fys::arena::data::CONTENDER} == pol._base_speed.at(5));
			REQUIRE(fys::arena::data::priority_elem(2, 19, fys::arena::data::PARTY_MEMBER) == pol._base_speed.at(4));
			REQUIRE(fys::arena::data::priority_elem(6, 15, fys::arena::data::PARTY_MEMBER) == pol._base_speed.at(3));
			REQUIRE(fys::arena::data::priority_elem(1, 10, fys::arena::data::CONTENDER) == pol._base_speed.at(2));
			REQUIRE(fys::arena::data::priority_elem(3, 3, fys::arena::data::PARTY_MEMBER) == pol._base_speed.at(1));
			REQUIRE(fys::arena::data::priority_elem(4, 2, fys::arena::data::CONTENDER) == pol._base_speed.at(0));

			pol.add_participant_in_list(7, 17, fys::arena::data::PARTY_MEMBER);
			pol.add_participant_in_list(8, 18, fys::arena::data::CONTENDER);
			pol.add_participant_in_list(9, 20, fys::arena::data::PARTY_MEMBER);

			REQUIRE(9 == pol._base_speed.size());
			REQUIRE(fys::arena::data::priority_elem{5, 30, fys::arena::data::CONTENDER} == pol._base_speed.at(8));
			REQUIRE(fys::arena::data::priority_elem(9, 20, fys::arena::data::PARTY_MEMBER) == pol._base_speed.at(7));
			REQUIRE(fys::arena::data::priority_elem(2, 19, fys::arena::data::PARTY_MEMBER) == pol._base_speed.at(6));
			REQUIRE(fys::arena::data::priority_elem(8, 18, fys::arena::data::CONTENDER) == pol._base_speed.at(5));
			REQUIRE(fys::arena::data::priority_elem(7, 17, fys::arena::data::PARTY_MEMBER) == pol._base_speed.at(4));
			REQUIRE(fys::arena::data::priority_elem(6, 15, fys::arena::data::PARTY_MEMBER) == pol._base_speed.at(3));
			REQUIRE(fys::arena::data::priority_elem(1, 10, fys::arena::data::CONTENDER) == pol._base_speed.at(2));
			REQUIRE(fys::arena::data::priority_elem(3, 3, fys::arena::data::PARTY_MEMBER) == pol._base_speed.at(1));
			REQUIRE(fys::arena::data::priority_elem(4, 2, fys::arena::data::CONTENDER) == pol._base_speed.at(0));

		} // End section : addParticipantInList test

		SECTION("removeParticipantFromList test") {

			REQUIRE(5 == pol._priorityList.size());
			REQUIRE(fys::arena::data::priority_elem{5, 30, fys::arena::data::CONTENDER} == pol._base_speed.at(4));
			REQUIRE(fys::arena::data::priority_elem(2, 19, fys::arena::data::PARTY_MEMBER) == pol._base_speed.at(3));
			REQUIRE(fys::arena::data::priority_elem(1, 10, fys::arena::data::CONTENDER) == pol._base_speed.at(2));
			REQUIRE(fys::arena::data::priority_elem(3, 3, fys::arena::data::PARTY_MEMBER) == pol._base_speed.at(1));
			REQUIRE(fys::arena::data::priority_elem(4, 2, fys::arena::data::CONTENDER) == pol._base_speed.at(0));

			pol.rm_participant_from_list(19, fys::arena::data::CONTENDER);// not exist
			REQUIRE(5 == pol._priorityList.size());

			pol.rm_participant_from_list(1, fys::arena::data::PARTY_MEMBER); // no party member with id 1, not exist
			REQUIRE(5 == pol._priorityList.size());

			pol.rm_participant_from_list(1, fys::arena::data::CONTENDER);
			REQUIRE(4 == pol._priorityList.size());
			REQUIRE(fys::arena::data::priority_elem{5, 30, fys::arena::data::CONTENDER} == pol._base_speed.at(3));
			REQUIRE(fys::arena::data::priority_elem(2, 19, fys::arena::data::PARTY_MEMBER) == pol._base_speed.at(2));
			REQUIRE(fys::arena::data::priority_elem(3, 3, fys::arena::data::PARTY_MEMBER) == pol._base_speed.at(1));
			REQUIRE(fys::arena::data::priority_elem(4, 2, fys::arena::data::CONTENDER) == pol._base_speed.at(0));

			pol.rm_participant_from_list(2, fys::arena::data::PARTY_MEMBER);
			pol.rm_participant_from_list(3, fys::arena::data::PARTY_MEMBER);
			REQUIRE(2 == pol._priorityList.size());
			REQUIRE(fys::arena::data::priority_elem{5, 30, fys::arena::data::CONTENDER} == pol._base_speed.at(1));
			REQUIRE(fys::arena::data::priority_elem(4, 2, fys::arena::data::CONTENDER) == pol._base_speed.at(0));

		} // End section : removeParticipantFromList test

	} // End section : Public API test

}

TEST_CASE("PriorityOrderListTestCase full turn test", "[service][arena]")
{

	PriorityOrderListPassThrough priorityList;

	REQUIRE(priorityList.empty());

	priorityList.add_participant_in_list(1, 3, fys::arena::data::PARTY_MEMBER);
	priorityList.add_participant_in_list(2, 5, fys::arena::data::PARTY_MEMBER);
	priorityList.add_participant_in_list(0, 8, fys::arena::data::CONTENDER);
	priorityList.add_participant_in_list(3, 10, fys::arena::data::PARTY_MEMBER);
	priorityList.add_participant_in_list(4, 20, fys::arena::data::PARTY_MEMBER);

	REQUIRE_FALSE(priorityList.empty());
	REQUIRE(fys::arena::data::priority_elem(4, 20, fys::arena::data::PARTY_MEMBER) == priorityList._base_speed.at(4));
	REQUIRE(fys::arena::data::priority_elem(3, 10, fys::arena::data::PARTY_MEMBER) == priorityList._base_speed.at(3));
	REQUIRE(fys::arena::data::priority_elem(0, 8, fys::arena::data::CONTENDER) == priorityList._base_speed.at(2));
	REQUIRE(fys::arena::data::priority_elem(2, 5, fys::arena::data::PARTY_MEMBER) == priorityList._base_speed.at(1));
	REQUIRE(fys::arena::data::priority_elem(1, 3, fys::arena::data::PARTY_MEMBER) == priorityList._base_speed.at(0));

	// TURN 1

	/**
	 * a4:20
	 * a3:10
	 * c0:8
	 * a2:5
	 * a1:3
	 */
	REQUIRE(1 == priorityList.get_turn_number());
	REQUIRE(fys::arena::data::priority_elem(4, DONT_CARE, fys::arena::data::PARTY_MEMBER) == priorityList.next());
	REQUIRE(fys::arena::data::priority_elem(3, DONT_CARE, fys::arena::data::PARTY_MEMBER) == priorityList.next());
	REQUIRE(fys::arena::data::priority_elem(0, DONT_CARE, fys::arena::data::CONTENDER) == priorityList.next());
	REQUIRE(fys::arena::data::priority_elem(2, DONT_CARE, fys::arena::data::PARTY_MEMBER) == priorityList.next());
	REQUIRE(fys::arena::data::priority_elem(1, DONT_CARE, fys::arena::data::PARTY_MEMBER) == priorityList.next());

	/**
	 * a4:20 -->  20 - 10  -->  a4:10     a4 Added
	 * a3:10 -----------------
	 * c0:8  -----------------
	 * a2:5  -----------------
	 * a1:3  -----------------
	 *
	 * a4:10 -->  10 - 10  -->  a4:0      a4 Added
	 * a3:10 -----------------
	 * c0:8  -----------------
	 * a2:5  -----------------
	 * a1:3  -----------------
	 *
	 * a3:10 -->  10 - 8  -->  a3:2      a3 Added
	 * c0:8  -----------------
	 * a2:5  -----------------
	 * a1:3  -----------------
	 * a4:0  -----------------
	 *
	 * c0:8  -->   8 - 5   -->  c0:3      c0 Added
	 * a2:5  -----------------
	 * a1:3  -----------------
	 * a3:2  -----------------
	 * a4:0  -----------------
	 *
	 * a2:5  -->   5 - 3   -->  a2:2      a2 Added
	 * c0:3  -----------------
	 * a1:3  -----------------
	 * a3:2  -----------------
	 * a4:0  -----------------
	 *
	 * c0:3  -->   3 - 3   -->  c0:1      c0 Added
	 * a1:3  -----------------
	 * a3:2  -----------------
	 * a2:2  -----------------
	 * a4:0  -----------------
	 *
	 * a1:3  -->   3 - 2   -->  a1:1      a1 Added
	 * a3:2  -----------------
	 * a2:2  -----------------
	 * a4:0  -----------------
	 * c0:0  -----------------
	 *
	 * resume a4, a4, a3, c0, a2, c0, a1
	 */

	REQUIRE(fys::arena::data::priority_elem(4, DONT_CARE, fys::arena::data::PARTY_MEMBER) == priorityList.next());
	REQUIRE(2 == priorityList.get_turn_number());
	REQUIRE(fys::arena::data::priority_elem(4, DONT_CARE, fys::arena::data::PARTY_MEMBER) == priorityList.next());
	REQUIRE(fys::arena::data::priority_elem(3, DONT_CARE, fys::arena::data::PARTY_MEMBER) == priorityList.next());
	REQUIRE(fys::arena::data::priority_elem(0, DONT_CARE, fys::arena::data::CONTENDER) == priorityList.next());
	REQUIRE(fys::arena::data::priority_elem(2, DONT_CARE, fys::arena::data::PARTY_MEMBER) == priorityList.next());
	REQUIRE(fys::arena::data::priority_elem(0, DONT_CARE, fys::arena::data::CONTENDER) == priorityList.next());
	REQUIRE(fys::arena::data::priority_elem(1, DONT_CARE, fys::arena::data::PARTY_MEMBER) == priorityList.next());

	/**
	 * a4:40  -->  40 - 32  -->  a4:8      a4 Added
	 * a3:32  -----------------
	 * c0:29  -----------------
	 * a2:27  -----------------
	 * a1:25  -----------------
	 *
	 * a3:32  -->  32 - 29  -->  a3:4      a3 Added
	 * c0:29  -----------------
	 * a2:27  -----------------
	 * a1:25  -----------------
	 * a4:8   -----------------
	 *
	 * c0:29  -->  29 - 27  -->  c0:2      c0 Added
	 * a2:27  -----------------
	 * a1:25  -----------------
	 * a4:8   -----------------
	 * a3:4   -----------------
	 *
	 * a2:27  -->  25 - 25  -->  a2:2      a2 Added
	 * a1:25  -----------------
	 * a4:8   -----------------
	 * a3:4   -----------------
	 * c0:2   -----------------
	 *
	 * a1:25  -->  25 - 8  -->  a1:17      a1 Added
	 * a4:8   -----------------
	 * a3:4   -----------------
	 * c0:2   -----------------
	 * a2:2   -----------------
	 *
	 */
	REQUIRE(fys::arena::data::priority_elem(4, DONT_CARE, fys::arena::data::PARTY_MEMBER) == priorityList.next());
	REQUIRE(3 == priorityList.get_turn_number());
	REQUIRE(fys::arena::data::priority_elem(3, DONT_CARE, fys::arena::data::PARTY_MEMBER) == priorityList.next());
	REQUIRE(fys::arena::data::priority_elem(0, DONT_CARE, fys::arena::data::CONTENDER) == priorityList.next());
	REQUIRE(fys::arena::data::priority_elem(2, DONT_CARE, fys::arena::data::PARTY_MEMBER) == priorityList.next());
	REQUIRE(fys::arena::data::priority_elem(1, DONT_CARE, fys::arena::data::PARTY_MEMBER) == priorityList.next());

} // End test case : Test for FightingPit testcase

#undef private

