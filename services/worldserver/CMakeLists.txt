cmake_minimum_required( VERSION 3.6 )
project(worldserver_service CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

find_package(nlohmann_json REQUIRED)
find_package(cppzmq REQUIRED)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/../common/include)

add_executable(worldserver_service
        ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../common/src/ServiceContextBase.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../common/include/ServiceContextBase.hh
        src/engine/WorldServerEngine.cpp
        include/engine/WorldServerEngine.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/ConnectionHandler.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/ConnectionHandler.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/WorldServerService.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/WorldServerService.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/WorldServerContext.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/WorldServerContext.hh)

target_link_libraries(worldserver_service cppzmq)

target_compile_options(worldserver_service PRIVATE -O2 -fstack-protector-all -g)
target_compile_options(worldserver_service PRIVATE -Wall -Wpedantic -Wextra -Wno-deprecated-declarations)

message( Compile worldserver_service : STATUS Set Warnings flag : ${CMAKE_CXX_FLAGS})
