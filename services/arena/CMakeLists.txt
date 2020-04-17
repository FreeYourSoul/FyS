cmake_minimum_required(VERSION 3.6...3.15)
if (${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else ()
    cmake_policy(VERSION 3.15)
endif ()

project(arena_service
        VERSION 1.0
        DESCRIPTION "Arena Service Server"
        LANGUAGES CXX)

include(${CMAKE_CURRENT_LIST_DIR}/../common/CMakeLists.txt)

find_package(nlohmann_json REQUIRED)
find_package(cppzmq REQUIRED)
find_package(fmt REQUIRED)

find_program(CCACHE_FOUND ccache)
if (CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif (CCACHE_FOUND)

add_executable(arena_service
        ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ChaiRegister.cpp
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
        ${CMAKE_CURRENT_LIST_DIR}/src/ConnectionHandler.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/ConnectionHandler.hh
        ${CMAKE_CURRENT_LIST_DIR}/include/ChaiRegister.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/network/DBConnector.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/network/DBConnector.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/network/WorkerService.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/network/WorkerService.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/network/WorkerConnection.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/network/WorkerConnection.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/fightingPit/HexagonSide.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/HexagonSide.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/ArenaServerService.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/ArenaServerService.hh)

target_compile_definitions(arena_service PUBLIC -DCHAISCRIPT_NO_THREADS)
target_link_libraries(arena_service PRIVATE
        common_service
        cppzmq
        nlohmann_json::nlohmann_json
        fmt::fmt dl cml
        ${Flatbuffers_LIBRARIES})

set_target_properties(arena_service PROPERTIES
        CXX_STANDARD 20
        CMAKE_CXX_COMPILER /usr/bin/g++)

set_property(TARGET arena_service PROPERTY INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/../../scripting_cache/cml/include
        ${fmt_INCLUDE_DIRS}
        ${FLATBUFFER_INCLUDE_DIR})

include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if (result)
    set_target_properties(arena_service PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif ()

target_compile_options(arena_service PRIVATE -O2 -fstack-protector-all -g -lfmt)
target_compile_options(arena_service PRIVATE -Wall -Wpedantic -Wextra -Wno-deprecated-declarations)
