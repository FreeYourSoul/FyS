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
#include <fightingPit/PriorityOrderList.hh>

class PriorityOrderListPassThrough : public PriorityOrderList {
public:
    using PriorityOrderList::PriorityOrderList;
    using PriorityOrderList::calculatePriority;
    using PriorityOrderList::_priorityList;
    using PriorityOrderList::_analyzedList;
    using PriorityOrderList::_baseSpeed;
};

TEST_CASE("PriorityOrderList test", "[service][arena]") {
    std::vector<data::PriorityElem> baseSpeed = {
        // id, speed, isContender
        {  1,   10,   true  },
        {  2,   19,   false },
        {  3,    3,   true  },
        {  4,    1,   false },
        {  5,   30,   true  }
    };
    auto PriorityOrderListPassThrough pol(baseSpeed);

    SECTION("Check instantiation test") {

        REQUIRE(5 == pol._priorityList.size());
        REQUIRE(5 == pol._analyzedList.size());
        REQUIRE(PriorityElem(5, 30,  true), _baseSpeed.at(0));
        REQUIRE(PriorityElem(2, 19, false), _baseSpeed.at(1));
        REQUIRE(PriorityElem(1, 10,  true), _baseSpeed.at(2));
        REQUIRE(PriorityElem(3, 3,  false), _baseSpeed.at(3));
        REQUIRE(PriorityElem(4, 1,   true), _baseSpeed.at(4));
        for (std::size_t i = 0; i < _baseSpeed.size(); ++i) {
            REQUIRE(_baseSpeed.at(i) == _priorityList.at(i));
            REQUIRE(_baseSpeed.at(i) == _analyzedList.at(i));
        }

    } // End section : Check instantiation test

    SECTION("SortAndCalculatePriority normal setup test") {

        // Base Speed order 5>2>1>3>4
        // 5:30
        // 2:19
        // 1:10
        // 3:3 
        // 4:2
        REQUIRE(PriorityElem(5, 30, true)  == _analyzedList.at(0));
        REQUIRE(PriorityElem(2, 19, false) == _analyzedList.at(1));
        REQUIRE(PriorityElem(1, 10, true)  == _analyzedList.at(2));
        REQUIRE(PriorityElem(3, 3, false) == _analyzedList.at(3));
        REQUIRE(PriorityElem(4, 2, true)  == _analyzedList.at(4));

        SECTION("Turn one check (init)") {
            
            // 5:30  -->  30 - 19  --> 5:11     Added 5
            // 2:19  ----------------> 2:19 
            // 1:10  ----------------> 1:10
            // 3:3   ----------------> 3:3 
            // 4:2   ----------------> 4:2 
            //
            // 2:19 -->  19 - 10   --> 2:9      Added 2
            // 5:11 -----------------> 5:11
            // 1:10 -----------------> 1:10
            // 3:3  -----------------> 3:3
            // 4:2  -----------------> 4:2
            //
            // 5:11 -->  11 - 9    --> 5:2      Added 5
            // 1:10 -----------------> 1:10
            // 2:9  -----------------> 2:9
            // 3:3  -----------------> 3:3
            // 4:2  -----------------> 4:2
            //
            // 1:10 -->  10 - 3    --> 1:7      Added 1
            // 2:9  -----------------> 2:9
            // 3:3  -----------------> 3:3
            // 5:2  -----------------> 5:2
            // 4:2  -----------------> 4:2
            //
            // 2:9  -->  9 - 7     --> 2:2      Added 2
            // 1:7  -----------------> 1:7
            // 3:3  -----------------> 3:3
            // 5:2  -----------------> 5:2
            // 4:2  -----------------> 4:2
            //
            // 1:7  -->  7 - 3     --> 1:4      Added 1
            // 3:3  -----------------> 3:3
            // 5:2  -----------------> 5:2
            // 2:2  -----------------> 2:2      
            // 4:2  -----------------> 4:2
            //
            // 1:4  -->  4 - 3     --> 1:1      Added 1    
            // 3:3  -----------------> 3:3
            // 5:2  -----------------> 5:2
            // 2:2  -----------------> 2:2
            // 4:2  -----------------> 4:2
            //
            // 3:3  -->  3 - 2     --> 3:1      Added 3
            // 5:2  -----------------> 5:2
            // 2:2  -----------------> 2:2
            // 4:2  -----------------> 4:2
            // 1:1  -----------------> 1:1
            //
            // 5:2  -->  2 - 2     --> 5:0      Added 5
            // 2:2  -----------------> 2:2
            // 4:2  -----------------> 4:2
            // 1:1  -----------------> 1:1
            // 3:1  -----------------> 3:1
            //
            // 2:2  -->  2 - 1     --> 2:1      Added 2
            // 4:2  -----------------> 4:2
            // 1:1  -----------------> 1:1
            // 3:1  -----------------> 3:1
            // 5:0  -----------------> 5:0
            //                         .___.
            // 4:2  -->  2 - 0     --> |4:2|      Added 4 End of turn
            // 2:1  -----------------> |2:1|
            // 1:1  -----------------> |1:1|
            // 3:1  -----------------> |3:1|
            // 5:0  -----------------> |5:0|
            //                         '¯¯¯'

            REQUIRE(11 == _priorityList.size());

            REQUIRE(5 == _priorityList.at(0).id);
            REQUIRE(2 == _priorityList.at(1).id);
            REQUIRE(5 == _priorityList.at(2).id);
            REQUIRE(1 == _priorityList.at(3).id);
            REQUIRE(2 == _priorityList.at(4).id);
            REQUIRE(1 == _priorityList.at(5).id);
            REQUIRE(1 == _priorityList.at(6).id);
            REQUIRE(3 == _priorityList.at(7).id);
            REQUIRE(5 == _priorityList.at(8).id);
            REQUIRE(2 == _priorityList.at(9).id);
            REQUIRE(4 == _priorityList.at(10).id);

            REQUIRE(PriorityElem(4, 2, true)  == _analyzedList.at(0));
            REQUIRE(PriorityElem(2, 1, false) == _analyzedList.at(1));
            REQUIRE(PriorityElem(1, 1, true)  == _analyzedList.at(2));
            REQUIRE(PriorityElem(3, 1, false) == _analyzedList.at(3));
            REQUIRE(PriorityElem(5, 0, true)  == _analyzedList.at(4));

        } // End section : Turn one check (init)

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
        REQUIRE(PriorityElem(5, 60, true)  == _analyzedList.at(0));
        REQUIRE(PriorityElem(2, 50, false) == _analyzedList.at(1));
        REQUIRE(PriorityElem(1, 41, true)  == _analyzedList.at(2));
        REQUIRE(PriorityElem(3, 34, false) == _analyzedList.at(3));
        REQUIRE(PriorityElem(4, 34, true)  == _analyzedList.at(4));

        PriorityOrderListPassThrough.sortBaseAndCalculatePriority();

        SECTION("Turn two check") {
            // 5:60  -->  60 - 50  --> 5:10     Added 5
            // 2:50  ----------------> 2:50 
            // 1:41  ----------------> 1:41
            // 3:34  ----------------> 3:34 
            // 4:34  ----------------> 4:34
            //
            // 2:50  -->  50 - 41  --> 2:9      Added 2 
            // 1:41  ----------------> 1:41
            // 3:34  ----------------> 3:34 
            // 4:34  ----------------> 4:34
            // 5:10  ----------------> 5:10
            //
            // 1:41  -->  41 - 34  --> 1:7      Added 1
            // 3:34  ----------------> 3:34 
            // 4:34  ----------------> 4:34
            // 5:10  ----------------> 5:10            
            // 2:9   ----------------> 2:9
            //
            // 3:34  -->  34 - 34  --> 3:0      Added 3 
            // 4:34  ----------------> 4:34
            // 5:10  ----------------> 5:10            
            // 2:9   ----------------> 2:9            
            // 1:7   ----------------> 1:7
            //
            // 4:34  -->  34 - 10  --> 4:24     Added 4 End of turn
            // 5:10  ----------------> 5:10            
            // 2:9   ----------------> 2:9            
            // 1:7   ----------------> 1:7            
            // 3:0   ----------------> 3:0  

            REQUIRE(5 == _priorityList.size());

            REQUIRE(5 == _priorityList.at(0).id);
            REQUIRE(2 == _priorityList.at(1).id);
            REQUIRE(1 == _priorityList.at(2).id);
            REQUIRE(3 == _priorityList.at(3).id);
            REQUIRE(4 == _priorityList.at(4).id);

            REQUIRE(PriorityElem(4, 24, true) == _analyzedList.at(0));
            REQUIRE(PriorityElem(5, 10, true) == _analyzedList.at(1));
            REQUIRE(PriorityElem(2, 9, false) == _analyzedList.at(2));
            REQUIRE(PriorityElem(1, 7, true)  == _analyzedList.at(3));
            REQUIRE(PriorityElem(3, 0, false) == _analyzedList.at(4));

        } // End section : Turn two check
        
        //   END OF TURN CALCULATION 
        //      REST + BASE + MAX
        //
        // 4:24  --> 24+2 +30    --> 4:56
        // 2:9  -->  9 +19+30    --> 2:58 
        // 1:7  -->  7 +10+30    --> 1:54 
        // 3:0  -->  0 +3 +30    --> 3:33
        // 5:10 -->  10+30+30    --> 5:70 
        // ordered
        // 5:70
        // 2:58
        // 4:56
        // 1:54
        // 3:33
        REQUIRE(PriorityElem(5, 70, true)  == _analyzedList.at(0));
        REQUIRE(PriorityElem(2, 58, false) == _analyzedList.at(1));
        REQUIRE(PriorityElem(4, 56, true)  == _analyzedList.at(2));
        REQUIRE(PriorityElem(1, 54, true)  == _analyzedList.at(3));
        REQUIRE(PriorityElem(3, 33, false) == _analyzedList.at(4));

        PriorityOrderListPassThrough.sortBaseAndCalculatePriority();
        
        SECTION("Turn three check") {

            // 5:70  -->  70 - 58  --> 5:12     Added 5
            // 2:58  ----------------> 2:58
            // 4:56  ----------------> 4:56
            // 1:54  ----------------> 1:54
            // 3:33  ----------------> 3:33
            //
            // 2:58  -->  58 - 56  --> 2:2      Added 2
            // 4:56  ----------------> 4:56
            // 1:54  ----------------> 1:54
            // 3:33  ----------------> 3:33
            // 5:12  ----------------> 5:12
            //
            // 4:56  -->  56 - 54  --> 4:2      Added 4 End of turn
            // 1:54  ----------------> 1:54
            // 3:33  ----------------> 3:33
            // 5:12  ----------------> 5:12            
            // 2:2   ----------------> 2:2

            REQUIRE(3 == _priorityList.size());

            REQUIRE(5 == _priorityList.at(0).id);
            REQUIRE(2 == _priorityList.at(1).id);
            REQUIRE(4 == _priorityList.at(2).id);

            REQUIRE(PriorityElem(4, 2, true)   == _analyzedList.at(0));
            REQUIRE(PriorityElem(1, 54, true)  == _analyzedList.at(1));
            REQUIRE(PriorityElem(3, 33, false) == _analyzedList.at(2));
            REQUIRE(PriorityElem(5, 12, true)  == _analyzedList.at(3));
            REQUIRE(PriorityElem(2, 2, false)  == _analyzedList.at(4));

        } // End section : Turn three check

        //   END OF TURN CALCULATION 
        //      REST + BASE + MAX
        //
        // 4:2   -->  2 +2 +30    --> 4:34
        // 2:2   -->  2 +19+30    --> 2:41 
        // 1:54  -->  54+10+30    --> 1:94 
        // 3:33  -->  33+3 +30    --> 3:66
        // 5:12  -->  12+30+30    --> 5:77 
        // ordered
        // 1:94
        // 5:77
        // 3:66
        // 2:41
        // 4:34
        REQUIRE(PriorityElem(1, 94, true)  == _analyzedList.at(0));
        REQUIRE(PriorityElem(5, 77, true)  == _analyzedList.at(1));
        REQUIRE(PriorityElem(3, 66, false) == _analyzedList.at(2));
        REQUIRE(PriorityElem(2, 41, false) == _analyzedList.at(3));
        REQUIRE(PriorityElem(4, 34, true)  == _analyzedList.at(4));

        PriorityOrderListPassThrough.sortBaseAndCalculatePriority();

    } // End section : SortAndCalculatePriority normal setup test

} 

#undef private

