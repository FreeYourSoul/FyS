cmake_minimum_required( VERSION 3.6...3.15 )
if(${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.15)
endif()

project(cml CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

find_package(nlohmann_json  REQUIRED)
find_package(fmt)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

add_library(cml
        ${CMAKE_CURRENT_LIST_DIR}/src/Cml.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/Cml.hh
        ${CMAKE_CURRENT_LIST_DIR}/include/CmlKey.hh
        ${CMAKE_CURRENT_LIST_DIR}/include/CmlCopy.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/CmlCopy.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/CmlSqlite.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/CmlSqlite.hh)

target_link_libraries(cml fmt::fmt dl stdc++fs)

target_compile_options(cml PRIVATE -O2 -fstack-protector-all -g)
target_compile_options(cml PRIVATE -Wall -Wpedantic -Wextra -Wno-deprecated-declarations)

add_subdirectory(test)
