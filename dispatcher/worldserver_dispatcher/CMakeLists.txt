cmake_minimum_required( VERSION 3.6 )

project(worldserver_dispatcher CXX)

set(CMAKE_CXX_COMPILER /usr/bin/g++)

#find boost
find_package(Boost REQUIRED)
#find cppzmq wrapper, installed by make of cppzmq
find_package(cppzmq REQUIRED)

include_directories(BEFORE /usr/include/tclap)
include(${CMAKE_SOURCE_DIR}/../common/CMakeLists.txt)

add_executable(worldserver_dispatcher ${COMMON_SRC})
target_link_libraries(worldserver_dispatcher
        cppzmq
        ${Boost_LIBRARIES})
target_compile_options(worldserver_dispatcher PRIVATE -O2 -fstack-protector-all -g)
target_compile_options(worldserver_dispatcher PRIVATE -Wall -Wpedantic -Wextra -Wno-deprecated-declarations)
message( STATUS Set Warnings flag : ${CMAKE_CXX_FLAGS})
