cmake_minimum_required( VERSION 3.6 )
project(cml_test CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

find_package(Catch2 REQUIRED)
include(Catch)

enable_testing()

include_directories(${CMAKE_CURRENT_LIST_DIR}/../include)

add_executable(cml_test
        #source tested
        ${CMAKE_CURRENT_LIST_DIR}/../src/Cml.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../include/Cml.hh
        ${CMAKE_CURRENT_LIST_DIR}/../include/CmlKey.hh
        ${CMAKE_CURRENT_LIST_DIR}/../src/CmlSqlite.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../include/CmlSqlite.hh

        #test files
        ${CMAKE_CURRENT_LIST_DIR}/catchMain.cpp
        ${CMAKE_CURRENT_LIST_DIR}/BasicCmlTestCase.cpp
        )

target_link_libraries(cml_test Catch2::Catch2)
catch_discover_tests(cml_test)
