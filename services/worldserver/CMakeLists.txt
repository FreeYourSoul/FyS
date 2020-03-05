cmake_minimum_required( VERSION 3.6 )
project(worldserver_service CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

include(${CMAKE_CURRENT_LIST_DIR}/../common/CMakeLists.txt)

find_package(TMXLITE        REQUIRED)
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
        ${FLATBUFFER_INCLUDE_DIR}
        ${TMXLITE_HEADERS})

add_executable(worldserver_service ${COMMON_SRCS}
        ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/engine/PlayersData.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/PlayersData.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/engine/WorldServerEngine.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/WorldServerEngine.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/engine/CollisionMap.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/CollisionMap.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/ConnectionHandler.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/ConnectionHandler.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/WorldServerService.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/WorldServerService.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/WorldServerContext.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/WorldServerContext.hh)

target_link_libraries(worldserver_service nlohmann_json::nlohmann_json cppzmq ${TMXLITE_LIBRARIES} ${Flatbuffers_LIBRARIES})

target_compile_options(worldserver_service PRIVATE -O2 -fstack-protector-all -g)
target_compile_options(worldserver_service PRIVATE -Wall -Wpedantic -Wextra -Wno-deprecated-declarations)
