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

find_package(TMXLITE REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(cppzmq REQUIRED)
find_package(fmt REQUIRED)
find_package(Lua REQUIRED)

find_program(CCACHE_FOUND ccache)
if (CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif (CCACHE_FOUND)

add_executable(worldserver_service
        ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/engine/player_data.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/player_data.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/engine/engine.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/engine.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/engine/collision_map.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/flatbuffer_generator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/flatbuffer_generator.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/engine/script_engine.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/script_engine.hh
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/collision_map.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/connection_handler.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/connection_handler.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/world_server_service.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/world_server_service.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/world_server_context.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/world_server_context.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/engine/world_populator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/world_populator.hh
        )

target_link_libraries(worldserver_service
        nlohmann_json::nlohmann_json
        cppzmq
        lua
        common_service
        fmt::fmt dl cml
        ${TMXLITE_LIBRARIES} # to remove
        ${Flatbuffers_LIBRARIES})

set_target_properties(worldserver_service PROPERTIES
        CXX_STANDARD 17
        CMAKE_CXX_COMPILER /usr/bin/g++)

set_property(TARGET worldserver_service PROPERTY INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/../../scripting_cache/cml/include
        ${FLATBUFFER_INCLUDE_DIR}
        ${TMXLITE_HEADERS})


include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if (result)
    set_target_properties(arena_service PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif ()

target_compile_options(worldserver_service PRIVATE -O2 -fstack-protector-all -g)
target_compile_options(worldserver_service PRIVATE -Wall -Wpedantic -Wextra -Wno-deprecated-declarations)
