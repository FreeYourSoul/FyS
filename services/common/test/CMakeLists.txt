cmake_minimum_required(VERSION 3.5)
project(common_test CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(cppzmq REQUIRED)
find_package(Catch2 REQUIRED)
include(Catch)

enable_testing()

add_executable(common_test
        ${CMAKE_CURRENT_LIST_DIR}/catchMain.cpp
        ${CMAKE_CURRENT_LIST_DIR}/WSActionFlatbufferTestCase.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ArenaComFlatbufferTestCase.cpp
        ${CMAKE_CURRENT_LIST_DIR}/SizedQueueTestCase.cpp
        ${CMAKE_CURRENT_LIST_DIR}/AlgorithmTestCase.cpp
        ${CMAKE_CURRENT_LIST_DIR}/BoundaryMapTestCase.cpp
        )

include_directories(../flatbuffer/includes)
include_directories(../include)

target_link_libraries(common_test cppzmq Catch2::Catch2)
catch_discover_tests(common_test)
