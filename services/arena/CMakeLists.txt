project(arena_service
        VERSION 1.0
        DESCRIPTION "Arena Service Server"
        LANGUAGES CXX)

add_executable(arena_service
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/chai_register.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fightingPit/fighting_pit.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/fightingPit/fighting_pit.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fightingPit/fighting_pit_layout.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/fightingPit/fighting_pit_layout.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fightingPit/fighting_pit_announcer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/fightingPit/fighting_pit_announcer.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fightingPit/priority_order_list.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/fightingPit/priority_order_list.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/fightingPit/rewards.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fightingPit/side_battle.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/fightingPit/side_battle.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fightingPit/contender/pit_contenders.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/fightingPit/contender/pit_contenders.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fightingPit/contender/fighting_contender.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/fightingPit/contender/fighting_contender.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fightingPit/contender/contender_scripting.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/fightingPit/contender/contender_scripting.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fightingPit/team/ally_party_teams.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/fightingPit/team/ally_party_teams.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fightingPit/team/party_team.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/fightingPit/team/party_team.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fightingPit/team/team_member.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/fightingPit/team/team_member.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/fightingPit/rewards.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/arena_server_context.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/arena_server_context.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/util/FbUtility.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/util/ChaiUtility.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/flatbuffer_generator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/flatbuffer_generator.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/history_manager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/history_manager.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/chai_register.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/network/db_connector.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/network/db_connector.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/network/worker_service.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/network/worker_service.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/network/worker_connection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/network/worker_connection.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fightingPit/hexagon_side.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/fightingPit/hexagon_side.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/arena_server_service.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/arena_server_service.hh)

target_compile_definitions(arena_service PUBLIC -DCHAISCRIPT_NO_THREADS)
target_link_libraries(arena_service PUBLIC
        dl
        fys::common_service
        fys::cml)
target_compile_features(arena_service PRIVATE cxx_std_20)

target_include_directories(arena_service PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include)

include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if (result)
    set_target_properties(arena_service PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif ()

target_compile_options(arena_service PRIVATE -O2 -fno-diagnostics-show-caret -fstack-protector-all -g -lfmt)
target_compile_options(arena_service PRIVATE -Wall -Wpedantic -Wextra -Wno-deprecated-declarations)
target_compile_definitions(arena_service PUBLIC CHAISCRIPT_NO_THREADS -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)

install(TARGETS arena_service DESTINATION ${CMAKE_INSTALL_BINDIR})
