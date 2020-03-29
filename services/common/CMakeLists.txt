cmake_minimum_required( VERSION 3.6...3.15 )
if(${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.15)
endif()

project(common)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/../../cmake/modules/")
find_package(FlatBuffers REQUIRED)

set(FLAT_BUFFER_PATH ${CMAKE_CURRENT_LIST_DIR}/flatbuffer)
set(FLATBUFFER_INCLUDE_DIR "${FLAT_BUFFER_PATH}/includes")
set(FLATBUFFER_SCHEMA_DIR "${FLAT_BUFFER_PATH}/schemas")

if (TARGET generateFbs)
else ()
    message("Flatbuffer generation ${FLATBUFFER_INCLUDE_DIR}")
    build_flatbuffers("${FLAT_BUFFER_SCHEMAS}"
            "${FLATBUFFER_SCHEMA_DIR}"
            generateFbs
            ""
            "${FLATBUFFER_INCLUDE_DIR}"
            ""
            "")
endif ()

if (TARGET common_service)
else()
    set(FLAT_BUFFER_SCHEMAS
            ${FLATBUFFER_SCHEMA_DIR}/arena/ArenaFightAction.fbs
            ${FLATBUFFER_SCHEMA_DIR}/arena/ArenaServerValidateAuth.fbs
            ${FLATBUFFER_SCHEMA_DIR}/arena/ArenaServerAuth.fbs
            ${FLATBUFFER_SCHEMA_DIR}/arena/FightingPitEncounter.fbs
            ${FLATBUFFER_SCHEMA_DIR}/arena/PartyTeamStatus.fbs
            ${FLATBUFFER_SCHEMA_DIR}/ClientCom.fbs
            ${FLATBUFFER_SCHEMA_DIR}/Notifications.fbs
            ${FLATBUFFER_SCHEMA_DIR}/WSAction.fbs)

    add_library(common_service INTERFACE)

    target_include_directories(common_service INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/include)

    target_sources(common_service INTERFACE
                ${CMAKE_CURRENT_LIST_DIR}/flatbuffer/includes/FightingPitEncounter_generated.h
                ${CMAKE_CURRENT_LIST_DIR}/flatbuffer/includes/PartyTeamStatus_generated.h
                ${CMAKE_CURRENT_LIST_DIR}/flatbuffer/includes/ArenaFightAction_generated.h
                ${CMAKE_CURRENT_LIST_DIR}/flatbuffer/includes/ArenaServerValidateAuth_generated.h
                ${CMAKE_CURRENT_LIST_DIR}/flatbuffer/includes/ArenaServerAuth_generated.h
                ${CMAKE_CURRENT_LIST_DIR}/flatbuffer/includes/ClientCom_generated.h
                ${CMAKE_CURRENT_LIST_DIR}/flatbuffer/includes/Notifications_generated.h
                ${CMAKE_CURRENT_LIST_DIR}/flatbuffer/includes/WSAction_generated.h
                ${CMAKE_CURRENT_LIST_DIR}/src/RandomGenerator.cpp
                ${CMAKE_CURRENT_LIST_DIR}/src/ServiceContextBase.cpp
                ${CMAKE_CURRENT_LIST_DIR}/include/RandomGenerator.hh
                ${CMAKE_CURRENT_LIST_DIR}/include/SizedQueue.hh
                ${CMAKE_CURRENT_LIST_DIR}/include/BoundaryMap.hh
                ${CMAKE_CURRENT_LIST_DIR}/include/ServiceContextBase.hh
                ${CMAKE_CURRENT_LIST_DIR}/include/algorithm/algorithm.hh
                ${CMAKE_CURRENT_LIST_DIR}/include/Key.hh)

endif ()

include_directories(flatbuffer/includes)
