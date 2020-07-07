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
        ${COMMON_SRCS}
        ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/chai_register.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/fightingPit/fighting_pit.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/fighting_pit.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/fightingPit/fighting_pit_layout.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/fighting_pit_layout.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/fightingPit/fighting_pit_announcer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/fighting_pit_announcer.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/fightingPit/priority_order_list.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/priority_order_list.hh
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/rewards.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/fightingPit/side_battle.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/side_battle.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/fightingPit/contender/pit_contenders.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/contender/pit_contenders.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/fightingPit/contender/fighting_contender.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/contender/fighting_contender.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/fightingPit/contender/contender_scripting.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/contender/contender_scripting.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/fightingPit/team/ally_party_teams.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/team/ally_party_teams.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/fightingPit/team/party_team.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/team/party_team.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/fightingPit/team/team_member.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/team/team_member.hh
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/rewards.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/arena_server_context.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/arena_server_context.hh
        ${CMAKE_CURRENT_LIST_DIR}/include/util/FbUtility.hh
        ${CMAKE_CURRENT_LIST_DIR}/include/util/ChaiUtility.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/flatbuffer_generator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/flatbuffer_generator.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/history_manager.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/history_manager.hh
        ${CMAKE_CURRENT_LIST_DIR}/include/chai_register.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/network/db_connector.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/network/db_connector.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/network/worker_service.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/network/worker_service.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/network/worker_connection.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/network/worker_connection.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/fightingPit/hexagon_side.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/fightingPit/hexagon_side.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/arena_server_service.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/arena_server_service.hh)

target_compile_definitions(arena_service PUBLIC -DCHAISCRIPT_NO_THREADS)
target_link_libraries(arena_service PRIVATE
        common_service
        cppzmq
        nlohmann_json::nlohmann_json
        fmt::fmt dl cml
        ${Flatbuffers_LIBRARIES})

set_target_properties(arena_service PROPERTIES
        CXX_STANDARD 17
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
target_compile_definitions(arena_service PUBLIC CHAISCRIPT_NO_THREADS -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
