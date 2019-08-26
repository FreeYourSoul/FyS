cmake_minimum_required( VERSION 3.6 )
project(arena_service CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

include(${CMAKE_CURRENT_LIST_DIR}/../common/CMakeLists.txt)

find_package(nlohmann_json  REQUIRED)
find_package(cppzmq         REQUIRED)
find_package(fmt)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

include_directories(
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${COMMON_INCLUDE_DIR}
        ${FLATBUFFER_INCLUDE_DIR})

add_executable(arena_service ${COMMON_SRCS}
        ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/fightingPit/FightingPit.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/FightingPit.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/fightingPit/FightingPitLayout.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/FightingPitLayout.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/fightingPit/FightingPitAnnouncer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/FightingPitAnnouncer.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/fightingPit/PriorityOrderList.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/PriorityOrderList.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/fightingPit/SideBattle.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/SideBattle.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/fightingPit/contender/PitContenders.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/contender/PitContenders.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/fightingPit/contender/FightingContender.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/contender/FightingContender.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/fightingPit/contender/ContenderScripting.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/contender/ContenderScripting.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/fightingPit/team/AllyPartyTeams.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/team/AllyPartyTeams.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/fightingPit/team/PartyTeam.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/team/PartyTeam.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/fightingPit/team/TeamMember.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/team/TeamMember.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/ArenaServerContext.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/ArenaServerContext.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/network/ConnectionHandler.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/network/ConnectionHandler.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/network/WorkerService.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/network/WorkerService.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/network/WorkerConnection.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/network/WorkerConnection.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/fightingPit/HexagonSide.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/HexagonSide.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/ArenaServerService.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/ArenaServerService.hh)

target_link_libraries(arena_service cppzmq ${Flatbuffers_LIBRARIES} fmt::fmt dl)

target_compile_options(arena_service PRIVATE -O0 -fstack-protector-all -g)
target_compile_options(arena_service PRIVATE -Wall -Wpedantic -Wextra -Wno-deprecated-declarations)
