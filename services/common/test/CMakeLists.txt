cmake_minimum_required(VERSION 3.5)
project(common_test CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(cppzmq REQUIRED)
find_package(Catch2 3 REQUIRED)
include(Catch)

enable_testing()

include_directories(${COMMON_INCLUDES_DIR})

add_executable(common_test
        ${CMAKE_CURRENT_SOURCE_DIR}/ws_action_flatbuffer_test_case.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/arena_com_flatbuffer_test_case.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sized_queue_test_case.cpp
        )

target_link_libraries(common_test cppzmq Catch2::Catch2WithMain)
catch_discover_tests(common_test)
