cmake_minimum_required( VERSION 3.6 )
project(worldserver_dispatcher CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

find_package(nlohmann_json REQUIRED)

if (NOT RELATIVE_TO_COMMON)
    set(RELATIVE_TO_COMMON /../common/)

    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}${RELATIVE_TO_COMMON}cmake)
endif ()

include_directories(${CMAKE_CURRENT_LIST_DIR}/include/)

#include source code to compile
include(${CMAKE_SOURCE_DIR}${RELATIVE_TO_COMMON}CMakeLists.txt)

#Build executable
add_executable(worldserver_dispatcher
        ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/WorldServerContext.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/WorldServerContext.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/WorldServerDispatcherHandler.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/WorldServerDispatcherHandler.hh)

target_compile_options(worldserver_dispatcher PRIVATE -O2 -fstack-protector-all -g)
target_compile_options(worldserver_dispatcher PRIVATE -Wall -Wpedantic -Wextra -Wno-deprecated-declarations)

target_link_libraries(worldserver_dispatcher
        PRIVATE common_dispatcher nlohmann_json::nlohmann_json)

message( STATUS Set Warnings flag : ${CMAKE_CXX_FLAGS})
