cmake_minimum_required(VERSION 3.6...3.15)
if (${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else ()
    cmake_policy(VERSION 3.15)
endif ()

project(common)

find_package(FlatBuffers REQUIRED)

set(FLAT_BUFFER_PATH ${CMAKE_CURRENT_LIST_DIR}/flatbuffer)
set(FLATBUFFER_INCLUDE_DIR "${FLAT_BUFFER_PATH}/includes")
set(FLATBUFFER_SCHEMA_DIR "${FLAT_BUFFER_PATH}/schemas")

if (TARGET generateFbs)
else ()
    set(FLAT_BUFFER_SCHEMAS
            ${FLATBUFFER_SCHEMA_DIR}/arena/ArenaFightAction.fbs
            ${FLATBUFFER_SCHEMA_DIR}/arena/ArenaServerValidateAuth.fbs
            ${FLATBUFFER_SCHEMA_DIR}/arena/ArenaServerAuth.fbs
            ${FLATBUFFER_SCHEMA_DIR}/arena/FightingPitEncounter.fbs
            ${FLATBUFFER_SCHEMA_DIR}/arena/CharacterStatus.fbs
            ${FLATBUFFER_SCHEMA_DIR}/arena/FightingPitState.fbs
            ${FLATBUFFER_SCHEMA_DIR}/arena/ReplyFrame.fbs
            ${FLATBUFFER_SCHEMA_DIR}/arena/EndBattle.fbs
            ${FLATBUFFER_SCHEMA_DIR}/arena/ActionExecuted.fbs
            ${FLATBUFFER_SCHEMA_DIR}/auth/IncomingPlayerOnWs.fbs
            ${FLATBUFFER_SCHEMA_DIR}/inventory/InventoryRequest.fbs
            ${FLATBUFFER_SCHEMA_DIR}/inventory/UpdatePlayerSoulDraughtboard.fbs
            ${FLATBUFFER_SCHEMA_DIR}/inventory/RetrievePlayerSoulDraughtboard.fbs
            ${FLATBUFFER_SCHEMA_DIR}/inventory/RetrievePlayerInventory.fbs
            ${FLATBUFFER_SCHEMA_DIR}/inventory/ExchangeInventory.fbs
            ${FLATBUFFER_SCHEMA_DIR}/world/WSResponse.fbs
            ${FLATBUFFER_SCHEMA_DIR}/world/InterServerCom.fbs
            ${FLATBUFFER_SCHEMA_DIR}/world/AuthFrame.fbs
            ${FLATBUFFER_SCHEMA_DIR}/world/WSAction.fbs
            ${FLATBUFFER_SCHEMA_DIR}/Notifications.fbs)

    message("Flatbuffer generation ${FLATBUFFER_INCLUDE_DIR}")
    build_flatbuffers("${FLAT_BUFFER_SCHEMAS}"
            "${FLATBUFFER_SCHEMA_DIR}"
            generateFbs
            ""
            "${FLATBUFFER_INCLUDE_DIR}"
            ""
            "")
endif ()

set(COMMON_SRCS
        ${CMAKE_CURRENT_LIST_DIR}/src/random_generator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/service_context_base.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/connection_handler.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/connection_handler.hh
        ${CMAKE_CURRENT_LIST_DIR}/include/random_generator.hh
        ${CMAKE_CURRENT_LIST_DIR}/include/sized_queue.hh
        ${CMAKE_CURRENT_LIST_DIR}/include/boundary_map.hh
        ${CMAKE_CURRENT_LIST_DIR}/include/service_context_base.hh
        ${CMAKE_CURRENT_LIST_DIR}/include/algorithm/algorithm.hh
        ${CMAKE_CURRENT_LIST_DIR}/include/key.hh)
set(COMMON_INCLUDES_DIR
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/flatbuffer/includes)

add_library(common_service INTERFACE)
add_library(fys::common_service ALIAS common_service)
target_link_libraries(common_service INTERFACE
        fys::fil
        cppzmq
        fmt::fmt
        nlohmann_json::nlohmann_json)
target_include_directories(common_service INTERFACE ${COMMON_INCLUDES_DIR})
target_sources(common_service INTERFACE ${COMMON_SRCS})

include_directories(flatbuffer/includes)
