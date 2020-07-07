cmake_minimum_required(VERSION 3.6...3.15)
if (${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else ()
    cmake_policy(VERSION 3.15)
endif ()

project(world_server_test CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

find_package(cppzmq REQUIRED)
find_package(Catch2 REQUIRED)
find_package(Lua REQUIRED)
find_package(fmt REQUIRED)
#find_package(FSeam)
include(Catch REQUIRED)

include(${CMAKE_CURRENT_LIST_DIR}/../../common/CMakeLists.txt)

enable_testing()
include_directories(
        ${COMMON_INCLUDES_DIR}
        ${TMXLITE_HEADERS}
        ${CMAKE_CURRENT_LIST_DIR}/../include
        ${CMAKE_CURRENT_LIST_DIR}/../../../scripting_cache/cml/include)

add_executable(world_server_test
        ${CMAKE_CURRENT_LIST_DIR}/catchMain.cpp

        # Test files
        ${CMAKE_CURRENT_LIST_DIR}/WorldServerEngineTestCase.cpp
        ${CMAKE_CURRENT_LIST_DIR}/FlatbufferGeneratorTestCase.cpp

        # Sources
        ${CMAKE_CURRENT_LIST_DIR}/../src/flatbuffer_generator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/engine/player_data.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/engine/engine.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/engine/collision_map.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/engine/world_populator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/flatbuffer_generator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/engine/script_engine.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/connection_handler.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/world_server_service.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/world_server_context.cpp
        )

add_executable(world_server_lua_test
        #Test files
        ${CMAKE_CURRENT_LIST_DIR}/catchMain.cpp
        ${CMAKE_CURRENT_LIST_DIR}/script/SpawningPoint_0_TestCase.cpp
        ${CMAKE_CURRENT_LIST_DIR}/script/WorldPopulatorTestCase.cpp

        #Source files
        ${CMAKE_CURRENT_LIST_DIR}/../src/engine/player_data.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../include/engine/player_data.hh
        ${CMAKE_CURRENT_LIST_DIR}/../src/engine/engine.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../include/engine/engine.hh
        ${CMAKE_CURRENT_LIST_DIR}/../src/engine/collision_map.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/flatbuffer_generator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../include/flatbuffer_generator.hh
        ${CMAKE_CURRENT_LIST_DIR}/../src/engine/script_engine.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../include/engine/script_engine.hh
        ${CMAKE_CURRENT_LIST_DIR}/../include/engine/collision_map.hh
        ${CMAKE_CURRENT_LIST_DIR}/../src/connection_handler.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../include/connection_handler.hh
        ${CMAKE_CURRENT_LIST_DIR}/../src/world_server_service.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../include/world_server_service.hh
        ${CMAKE_CURRENT_LIST_DIR}/../src/world_server_context.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../include/world_server_context.hh
        ${CMAKE_CURRENT_LIST_DIR}/../src/engine/world_populator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../include/engine/world_populator.hh
        )

target_link_libraries(world_server_test cppzmq Catch2::Catch2)
target_link_libraries(world_server_test
        ${TMXLITE_LIBRARIES} # to remove
        cppzmq
        Catch2::Catch2
        fmt
        lua
        common_service)
catch_discover_tests(world_server_test)

target_link_libraries(world_server_lua_test
        ${TMXLITE_LIBRARIES} # to remove
        cppzmq
        Catch2::Catch2
        fmt
        lua
        common_service)
catch_discover_tests(world_server_lua_test)