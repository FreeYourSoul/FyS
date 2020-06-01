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
        ${CMAKE_CURRENT_LIST_DIR}/../../../scripting_cache/cml/include
        ${CMAKE_CURRENT_LIST_DIR}/../include)

add_executable(world_server_test
        ${CMAKE_CURRENT_LIST_DIR}/catchMain.cpp

        # Test files
        ${CMAKE_CURRENT_LIST_DIR}/WorldServerEngineTestCase.cpp
        ${CMAKE_CURRENT_LIST_DIR}/FlatbufferGeneratorTestCase.cpp

        # Sources
        ${CMAKE_CURRENT_LIST_DIR}/../src/FlatbufferGenerator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/engine/PlayersData.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/engine/WorldServerEngine.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/engine/CollisionMap.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/engine/WorldPopulator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/FlatbufferGenerator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/engine/ScriptEngine.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/ConnectionHandler.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/WorldServerService.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/WorldServerContext.cpp
        )

add_executable(world_server_lua_test
        #Test files
        ${CMAKE_CURRENT_LIST_DIR}/catchMain.cpp
        ${CMAKE_CURRENT_LIST_DIR}/script/SpawningPoint_0_TestCase.cpp

        )

target_link_libraries(world_server_test cppzmq Catch2::Catch2)
catch_discover_tests(world_server_test)

target_link_libraries(world_server_lua_test cppzmq Catch2::Catch2 fmt lua)
catch_discover_tests(world_server_lua_test)