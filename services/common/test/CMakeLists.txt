cmake_minimum_required(VERSION 3.5)
project(common_test CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(cppzmq REQUIRED)
find_package(Catch2 REQUIRED)
include(Catch)

enable_testing()

include_directories(${COMMON_INCLUDES_DIR})

add_executable(common_test
        ${CMAKE_CURRENT_LIST_DIR}/catchMain.cpp
        ${CMAKE_CURRENT_LIST_DIR}/WSActionFlatbufferTestCase.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ArenaComFlatbufferTestCase.cpp
        ${CMAKE_CURRENT_LIST_DIR}/SizedQueueTestCase.cpp
        )

target_link_libraries(common_test cppzmq Catch2::Catch2)
catch_discover_tests(common_test)
