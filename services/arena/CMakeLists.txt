cmake_minimum_required( VERSION 3.6 )
project(arena_service CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

include(${CMAKE_CURRENT_LIST_DIR}/../common/CMakeLists.txt)

find_package(nlohmann_json  REQUIRED)
find_package(cppzmq         REQUIRED)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

include_directories(
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${COMMON_INCLUDE_DIR}
        ${FLATBUFFER_INCLUDE_DIR})

add_executable(arena_service ${COMMON_SRCS}
        ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp)

target_link_libraries(arena_service cppzmq ${Flatbuffers_LIBRARIES})

target_compile_options(arena_service PRIVATE -O0 -fstack-protector-all -g)
target_compile_options(arena_service PRIVATE -Wall -Wpedantic -Wextra -Wno-deprecated-declarations)
