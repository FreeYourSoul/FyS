cmake_minimum_required( VERSION 3.6 )
project(quest_dispatcher CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

if (NOT RELATIVE_TO_COMMON)
    set(RELATIVE_TO_COMMON /../common/)

    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}${RELATIVE_TO_COMMON}cmake)
endif ()

message(CMAKE_MODULE_PATH : ${CMAKE_MODULE_PATH})

#include source code to compile
include(${CMAKE_SOURCE_DIR}${RELATIVE_TO_COMMON}CMakeLists.txt)

#Build executable
add_executable(quest_dispatcher
        ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp)

target_compile_options(quest_dispatcher PRIVATE -O2 -fstack-protector-all -g)
target_compile_options(quest_dispatcher PRIVATE -Wall -Wpedantic -Wextra -Wno-deprecated-declarations)

target_link_libraries(quest_dispatcher
        PRIVATE common_dispatcher)

message( STATUS Set Warnings flag : ${CMAKE_CXX_FLAGS})

