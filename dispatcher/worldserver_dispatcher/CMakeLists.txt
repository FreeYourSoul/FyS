cmake_minimum_required( VERSION 3.6 )
project(worldserver_dispatcher CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

#find
find_package(Boost REQUIRED)
find_package(TCLAP REQUIRED)

#include source code to compile
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
include(${CMAKE_SOURCE_DIR}/../common/CMakeLists.txt)

#Build executable
add_executable(worldserver_dispatcher
        ${CMAKE_SOURCE_DIR}/src/main.cpp
        ${CMAKE_SOURCE_DIR}/src/WorldServerDispatcherHandler.cpp
        ${CMAKE_SOURCE_DIR}/src/WorldServerDispatcherHandler.hh)

target_compile_options(worldserver_dispatcher PRIVATE -O2 -fstack-protector-all -g)
target_compile_options(worldserver_dispatcher PRIVATE -Wall -Wpedantic -Wextra -Wno-deprecated-declarations)

target_link_libraries(worldserver_dispatcher
        common_dispatcher
        ${CONAN_LIBS_ZMQ}
        ${Boost_LIBRARIES})

message( STATUS Set Warnings flag : ${CMAKE_CXX_FLAGS})
