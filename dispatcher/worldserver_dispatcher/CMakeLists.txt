cmake_minimum_required( VERSION 3.6 )
project(worldserver_dispatcher CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

#find boost
find_package(Boost REQUIRED)
#find cppzmq wrapper, installed by make of cppzmq
find_package(cppzmq REQUIRED)

#include source code to compile
include(${CMAKE_SOURCE_DIR}/../common/CMakeLists.txt)

#Build executable
add_executable(worldserver_dispatcher ${CMAKE_SOURCE_DIR}/main.cpp)

target_include_directories(worldserver_dispatcher
        PRIVATE /usr/include/tclap
        PRIVATE /usr/include/spdlog)

target_link_libraries(worldserver_dispatcher
        cppzmq
        common_dispatcher
        ${Boost_LIBRARIES})
target_compile_options(worldserver_dispatcher PRIVATE -O2 -fstack-protector-all -g)
target_compile_options(worldserver_dispatcher PRIVATE -Wall -Wpedantic -Wextra -Wno-deprecated-declarations)

message( STATUS Set Warnings flag : ${CMAKE_CXX_FLAGS})
