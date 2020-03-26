cmake_minimum_required(VERSION 3.6)
project(arena_test CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

find_package(cppzmq REQUIRED)
find_package(Catch2 REQUIRED)
find_package(fmt)
find_package(FSeam)
include(Catch)

include(${CMAKE_CURRENT_LIST_DIR}/../../common/CMakeLists.txt)

enable_testing()

include_directories(
        ${COMMON_INCLUDE_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/../../../scripting_cache/cml/include
        ${CMAKE_CURRENT_LIST_DIR}/../include)

addFSeamTests(
        DESTINATION_TARGET arena_test

        FILES_AS_SOURCE #source tested
        ${COMMON_SRCS}
        ${CMAKE_CURRENT_LIST_DIR}/../src/fightingPit/PriorityOrderList.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/fightingPit/HexagonSide.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/fightingPit/FightingPitAnnouncer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/fightingPit/FightingPitLayout.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/fightingPit/SideBattle.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/fightingPit/FightingPit.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/fightingPit/contender/FightingContender.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/fightingPit/contender/PitContenders.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/fightingPit/contender/ContenderScripting.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/fightingPit/team/AllyPartyTeams.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/fightingPit/team/TeamMember.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/fightingPit/team/PartyTeam.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/network/WorkerService.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/ChaiRegister.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/ArenaServerContext.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../include/ChaiRegister.hh
        ${CMAKE_CURRENT_LIST_DIR}/../include/fightingPit/team/PartyTeam.hh
        ${CMAKE_CURRENT_LIST_DIR}/../include/fightingPit/team/TeamMember.hh
        ${CMAKE_CURRENT_LIST_DIR}/../include/fightingPit/team/AllyPartyTeams.hh
        ${CMAKE_CURRENT_LIST_DIR}/../include/fightingPit/contender/ContenderScripting.hh
        ${CMAKE_CURRENT_LIST_DIR}/../include/fightingPit/contender/PitContenders.hh
        ${CMAKE_CURRENT_LIST_DIR}/../include/fightingPit/contender/FightingContender.hh
        ${CMAKE_CURRENT_LIST_DIR}/../include/fightingPit/FightingPit.hh
        ${CMAKE_CURRENT_LIST_DIR}/../include/fightingPit/SideBattle.hh
        ${CMAKE_CURRENT_LIST_DIR}/../include/fightingPit/FightingPitLayout.hh
        ${CMAKE_CURRENT_LIST_DIR}/../include/fightingPit/FightingPitAnnouncer.hh
        ${CMAKE_CURRENT_LIST_DIR}/../include/fightingPit/PriorityOrderList.hh
        ${CMAKE_CURRENT_LIST_DIR}/../include/fightingPit/HexagonSide.hh
        ${CMAKE_CURRENT_LIST_DIR}/../include/network/WorkerService.hh
        ${CMAKE_CURRENT_LIST_DIR}/../include/ArenaServerContext.hh

        FOLDER_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/../include

        TST_SRC #test files
        ${CMAKE_CURRENT_LIST_DIR}/catchMain.cpp
        ${CMAKE_CURRENT_LIST_DIR}/fightingPit/TestType.hh
        ${CMAKE_CURRENT_LIST_DIR}/fightingPit/WorkerServiceTestCase.cpp
        ${CMAKE_CURRENT_LIST_DIR}/fightingPit/HexagonSideTestCase.cpp
        ${CMAKE_CURRENT_LIST_DIR}/fightingPit/PriorityOrderListTestCase.cpp
        ${CMAKE_CURRENT_LIST_DIR}/fightingPit/FightingPitAnnouncerTestCase.cpp
        ${CMAKE_CURRENT_LIST_DIR}/fightingPit/FightingPitTestCase.cpp

        TO_MOCK
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/ConnectionHandler.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../../common/include/RandomGenerator.hh
)

addFSeamTests(
        DESTINATION_TARGET sampy_test

        FILES_AS_SOURCE #source tested
        ${COMMON_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/contender/ContenderScripting.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/contender/FightingContender.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/contender/PitContenders.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/team/AllyPartyTeams.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/team/PartyTeam.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/team/TeamMember.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/HexagonSide.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/ChaiRegister.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/ChaiRegister.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/contender/ContenderScripting.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/contender/FightingContender.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/contender/PitContenders.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/team/AllyPartyTeams.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/team/PartyTeam.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/team/TeamMember.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/HexagonSide.hh

        FOLDER_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/../include

        TST_SRC #test files
        ${CMAKE_CURRENT_SOURCE_DIR}/catchMain.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/scriptTests/Contender_SampyTestCase.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/scriptTests/ChaiRegisterTestCase.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/scriptTests/Action_DamageTestCase.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/scriptTests/Action_HealTestCase.cpp

        TO_MOCK
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/ConnectionHandler.hh
)
target_compile_definitions(sampy_test PUBLIC CHAISCRIPT_NO_THREADS)
target_link_libraries(sampy_test cppzmq fmt::fmt dl cml)
target_compile_definitions(arena_test PUBLIC -DCHAISCRIPT_NO_THREADS)
target_link_libraries(arena_test cppzmq Catch2::Catch2 fmt::fmt dl cml)
catch_discover_tests(arena_test)
