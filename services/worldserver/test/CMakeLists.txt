cmake_minimum_required(VERSION 3.6...3.15)
if (${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else ()
    cmake_policy(VERSION 3.15)
endif ()

project(world_server_test CXX)

set(CMAKE_CXX_COMPILER /usr/bin/g++)

get_target_property(WS_SRCS worldserver_service SOURCES)

add_executable(world_server_test
        ${CMAKE_CURRENT_SOURCE_DIR}/catch_main.cpp

        # Test files
        ${CMAKE_CURRENT_SOURCE_DIR}/world_server_engine_testcase.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/flatbuffer_generator_testcase.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/collision_map_testcase.cpp

        # Sources
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/engine/player_data.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/engine/player_data.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/engine/engine.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/engine/engine.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/engine/collision_map.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/flatbuffer_generator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/flatbuffer_generator.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/engine/script_engine.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/engine/script_engine.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/engine/collision_map.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/connection_handler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/connection_handler.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/world_server_service.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/world_server_service.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/world_server_context.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/world_server_context.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/engine/world_populator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/engine/world_populator.hh
        )

add_executable(world_server_lua_test
        #Test files
        ${CMAKE_CURRENT_SOURCE_DIR}/catch_main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/script/spawning_point_0_testcase.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/script/world_populator_testcase.cpp

        #Source files

        ${CMAKE_CURRENT_SOURCE_DIR}/../src/engine/player_data.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/engine/player_data.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/engine/engine.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/engine/engine.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/engine/collision_map.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/flatbuffer_generator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/flatbuffer_generator.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/engine/script_engine.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/engine/script_engine.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/engine/collision_map.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/connection_handler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/connection_handler.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/world_server_service.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/world_server_service.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/world_server_context.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/world_server_context.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/engine/world_populator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/engine/world_populator.hh
        )

target_link_libraries(world_server_test PUBLIC
        Catch2::Catch2
        fys::common_service
        fys::collision_map
        fys::cml
        lua
        ${TMXLITE_LIBRARIES} # to remove
        )
target_include_directories(world_server_test PRIVATE
        ${COMMON_INCLUDES_DIR}
        ${TMXLITE_HEADERS}
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../collision_map)
catch_discover_tests(world_server_test)

target_link_libraries(world_server_lua_test
        Catch2::Catch2
        fys::common_service
        fys::cml
        lua)
target_include_directories(world_server_lua_test PRIVATE
        ${COMMON_INCLUDES_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../collision_map)
catch_discover_tests(world_server_lua_test)