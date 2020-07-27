cmake_minimum_required(VERSION 3.6...3.15)
if (${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else ()
    cmake_policy(VERSION 3.15)
endif ()

project(chatserver_service
        VERSION 1.0
        DESCRIPTION "Chat Service Server"
        LANGUAGES CXX)

find_package(TMXLITE REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(cppzmq REQUIRED)
find_package(fmt REQUIRED)

find_program(CCACHE_FOUND ccache)
if (CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif (CCACHE_FOUND)

add_executable(chatserver_service
        ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/chat_engine.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/chat_engine.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/chatserver_context.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/chatserver_context.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/chat_room.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/chat_room.hh
        )

target_link_libraries(worldserver_service
        nlohmann_json::nlohmann_json
        cppzmq
        lua
        fys::common_service
        fmt::fmt dl cml
        ${TMXLITE_LIBRARIES} # to remove
        ${Flatbuffers_LIBRARIES})