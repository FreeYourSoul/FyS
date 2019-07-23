cmake_minimum_required( VERSION 3.6 )
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

cmake_minimum_required(VERSION 3.5)
project(arena_test CXX)

set (CMAKE_CXX_STANDARD 17)

find_package(cppzmq REQUIRED)
find_package(Catch2 REQUIRED)
include(Catch)

enable_testing()

include_directories(${CMAKE_CURRENT_LIST_DIR}/../include)

add_executable(arena_test
        ${CMAKE_CURRENT_LIST_DIR}/catchMain.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../src/fightingPit/HexagonSide.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../include/fightingPit/HexagonSide.hh
        ${CMAKE_CURRENT_LIST_DIR}/HexagonSideTestCase.cpp
        )

target_link_libraries(arena_test cppzmq Catch2::Catch2)
catch_discover_tests(arena_test)
