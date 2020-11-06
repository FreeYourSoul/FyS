cmake_minimum_required(VERSION 3.6...3.15)
if (${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else ()
    cmake_policy(VERSION 3.15)
endif ()

project(world_server_test CXX)

set(CMAKE_CXX_COMPILER /usr/bin/g++)

include_directories(
        ${COMMON_INCLUDES_DIR}
        ${TMXLITE_HEADERS}
        ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(world_server_test
        ${CMAKE_CURRENT_SOURCE_DIR}/catchMain.cpp

        # Test files
        ${CMAKE_CURRENT_SOURCE_DIR}/WorldServerEngineTestCase.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/FlatbufferGeneratorTestCase.cpp

        # Sources
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/flatbuffer_generator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/engine/player_data.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/engine/engine.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/engine/collision_map.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/engine/world_populator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/flatbuffer_generator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/engine/script_engine.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/connection_handler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/world_server_service.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/world_server_context.cpp
        )

add_executable(world_server_lua_test
        #Test files
        ${CMAKE_CURRENT_SOURCE_DIR}/catchMain.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/script/SpawningPoint_0_TestCase.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/script/WorldPopulatorTestCase.cpp

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
        fys::cml
        lua
        ${TMXLITE_LIBRARIES} # to remove
        )

catch_discover_tests(world_server_test)

target_link_libraries(world_server_lua_test
        Catch2::Catch2
        fys::common_service
        fys::cml
        ${TMXLITE_LIBRARIES} # to remove
        lua)
catch_discover_tests(world_server_lua_test)