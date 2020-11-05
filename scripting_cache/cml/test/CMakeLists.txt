cmake_minimum_required(VERSION 3.6...3.15)
if (${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else ()
    cmake_policy(VERSION 3.15)
endif ()

project(cml_test CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

find_package(Catch2 REQUIRED)
include(Catch)

enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(cml_test
        #source tested
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/Cml.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/Cml.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/CmlKey.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/CmlCopy.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/CmlCopy.cpp

        #test files
        ${CMAKE_CURRENT_SOURCE_DIR}/catchMain.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BasicCmlTestCase.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/CmlCopyTestCase.cpp
        )
set(CMAKE_BUILD_TYPE Debug)
target_link_libraries(cml_test Catch2::Catch2 stdc++fs)
catch_discover_tests(cml_test)
