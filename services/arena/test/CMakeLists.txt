project(arena_test CXX)

addFSeamTests(
        DESTINATION_TARGET arena_test

        FILES_AS_SOURCE #source tested
        ${COMMON_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/util/chai_utility.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/priority_order_list.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/hexagon_side.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/fighting_pit_announcer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/fighting_pit_layout.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/side_battle.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/fighting_pit.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/contender/fighting_contender.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/contender/pit_contenders.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/contender/contender_scripting.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/team/ally_party_teams.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/team/team_member.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/team/party_team.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/network/worker_service.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/arena_server_context.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/chai_register.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/flatbuffer_generator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/history_manager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/history_manager.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/flatbuffer_generator.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/util/fb_utility.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/chai_register.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/team/party_team.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/team/team_member.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/rewards.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/team/ally_party_teams.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/contender/contender_scripting.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/contender/pit_contenders.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/contender/fighting_contender.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/fighting_pit.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/side_battle.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/fighting_pit_layout.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/fighting_pit_announcer.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/priority_order_list.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/hexagon_side.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/network/worker_service.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/arena_server_context.hh

        FOLDER_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/../include

        TST_SRC #test files
        ${CMAKE_CURRENT_SOURCE_DIR}/catch_main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/fightingPit/test_type.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/fightingPit/worker_service_test_case.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/fightingPit/flatbuffer_generator_test_case.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/fightingPit/hexagon_side_test_case.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/fightingPit/priority_order_list_test_case.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/fightingPit/fighting_pit_announcer_test_case.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/fightingPit/common_types_test_case.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/fightingPit/fighting_pit_test_case.cpp

        TO_MOCK
        ${CMAKE_CURRENT_SOURCE_DIR}/../../common/include/connection_handler.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../../common/include/random_generator.hh
)

addFSeamTests(
        DESTINATION_TARGET sampy_test

        FILES_AS_SOURCE #source tested
        ${COMMON_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/catch_main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/util/chai_utility.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/contender/contender_scripting.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/contender/fighting_contender.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/contender/pit_contenders.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/team/ally_party_teams.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/team/party_team.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/team/team_member.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/side_battle.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/hexagon_side.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/fighting_pit_layout.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/fightingPit/priority_order_list.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/chai_register.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/flatbuffer_generator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/history_manager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/history_manager.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/flatbuffer_generator.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/util/fb_utility.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/chai_register.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/rewards.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/side_battle.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/priority_order_list.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/fighting_pit_layout.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/contender/contender_scripting.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/contender/fighting_contender.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/contender/pit_contenders.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/team/ally_party_teams.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/team/party_team.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/team/team_member.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/fightingPit/hexagon_side.hh

        FOLDER_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/../include

        TST_SRC #test files
        ${CMAKE_CURRENT_SOURCE_DIR}/scriptTests/contender_sampy_test_case.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/scriptTests/chai_register_test_case.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/scriptTests/action_damage_test_case.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/scriptTests/action_heal_test_case.cpp

        TO_MOCK
        ${CMAKE_CURRENT_SOURCE_DIR}/../../common/include/connection_handler.hh
)

target_compile_definitions(sampy_test PUBLIC CHAISCRIPT_NO_THREADS SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
target_link_libraries(sampy_test cppzmq dl fmt::fmt fys::cml nlohmann_json::nlohmann_json)
target_compile_features(sampy_test PRIVATE cxx_std_23)
target_compile_options(sampy_test PRIVATE -fno-diagnostics-show-caret )
target_include_directories(sampy_test PRIVATE
        ${COMMON_INCLUDES_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripting_cache/cml/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../include)

target_compile_definitions(arena_test PUBLIC CHAISCRIPT_NO_THREADS SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
target_link_libraries(arena_test cppzmq Catch2::Catch2 fmt::fmt dl fys::cml nlohmann_json::nlohmann_json)
target_compile_features(arena_test PRIVATE cxx_std_23)
target_compile_options(arena_test PRIVATE -fno-diagnostics-show-caret )
target_include_directories(arena_test PRIVATE
        ${COMMON_INCLUDES_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripting_cache/cml/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../include)
