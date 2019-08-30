cmake_minimum_required( VERSION 3.6 )
project(arena_test CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER /usr/bin/g++)


find_package(FSeam)
find_package(cppzmq REQUIRED)
find_package(Catch2 REQUIRED)
include(Catch)

enable_testing()

include_directories(${CMAKE_CURRENT_LIST_DIR}/../include)

add_executable(arena_test
        #source tested
        ${CMAKE_CURRENT_LIST_DIR}/../src/fightingPit/PriorityOrderList.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../include/fightingPit/PriorityOrderList.hh
        ${CMAKE_CURRENT_LIST_DIR}/../src/fightingPit/HexagonSide.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../include/fightingPit/HexagonSide.hh

        #test files
        ${CMAKE_CURRENT_LIST_DIR}/catchMain.cpp
        ${CMAKE_CURRENT_LIST_DIR}/fightingPit/HexagonSideTestCase.cpp
        ${CMAKE_CURRENT_LIST_DIR}/fightingPit/PriorityOrderListTestCase.cpp
)

addFSeamTests(
        DESTINATION_TARGET sampy_test

        FILES_AS_SOURCE #source tested
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/contender/ContenderScripting.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/contender/FightingContender.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/contender/PitContender.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/team/AllyPartyTeams.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/team/PartyTeam.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/team/TeamMember.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/contender/ContenderScripting.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/contender/FightingContender.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/contender/PitContender.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/team/AllyPartyTeams.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/team/PartyTeam.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/team/TeamMember.hh     

        FOLDER_INCLUDES  ${CMAKE_CURRENT_SOURCE_DIR}/../include

        TST_SRC #test files
        ${CMAKE_CURRENT_SOURCE_DIR}/catchMain.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Contender_SampyTestCase.cpp

        TO_MOCK
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/network/ConnectionHandler.hh
)

target_link_libraries(arena_test cppzmq Catch2::Catch2)
catch_discover_tests(arena_test)
