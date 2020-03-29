cmake_minimum_required(VERSION 3.6...3.15)
if (${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else ()
    cmake_policy(VERSION 3.15)
endif ()

project(worldserver_service
        VERSION 1.0
        DESCRIPTION "WorldServer Service Server"
        LANGUAGES CXX)

include(${CMAKE_CURRENT_LIST_DIR}/../common/CMakeLists.txt)

find_package(TMXLITE REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(cppzmq REQUIRED)

find_program(CCACHE_FOUND ccache)
if (CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif (CCACHE_FOUND)

add_executable(worldserver_service
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

target_link_libraries(worldserver_service
        nlohmann_json::nlohmann_json
        cppzmq
        common_service
        ${TMXLITE_LIBRARIES}
        ${Flatbuffers_LIBRARIES})

set_target_properties(worldserver_service PROPERTIES
        CXX_STANDARD 20
        CMAKE_CXX_COMPILER /usr/bin/g++)

set_property(TARGET worldserver_service PROPERTY INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${FLATBUFFER_INCLUDE_DIR}
        ${TMXLITE_HEADERS})


include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if (result)
    set_target_properties(arena_service PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif ()

target_compile_options(worldserver_service PRIVATE -O2 -fstack-protector-all -g)
target_compile_options(worldserver_service PRIVATE -Wall -Wpedantic -Wextra -Wno-deprecated-declarations)
