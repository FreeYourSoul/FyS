cmake_minimum_required( VERSION 3.6...3.15 )
if(${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.15)
endif()

project(arena_dispatcher
        VERSION 1.0
        DESCRIPTION "Arena Dispatcher"
        LANGUAGES CXX)

find_package(nlohmann_json REQUIRED)

if (NOT RELATIVE_TO_COMMON)
    set(RELATIVE_TO_COMMON /../common/)

    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}${RELATIVE_TO_COMMON}cmake)
endif ()

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

#include source code to compile
include(${CMAKE_SOURCE_DIR}${RELATIVE_TO_COMMON}CMakeLists.txt)

#Build executable
add_executable(arena_dispatcher
        ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp)

target_compile_options(arena_dispatcher PRIVATE -O2 -fstack-protector-all -g)
target_compile_options(arena_dispatcher PRIVATE -Wall -Wpedantic -Wextra -Wno-deprecated-declarations)

target_link_libraries(arena_dispatcher
        PRIVATE common_dispatcher nlohmann_json::nlohmann_json)

set_target_properties(arena_dispatcher PROPERTIES
        CXX_STANDARD 20
        CMAKE_CXX_COMPILER /usr/bin/g++)

message( STATUS Set Warnings flag : ${CMAKE_CXX_FLAGS})