cmake_minimum_required( VERSION 3.6 )

project(common)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/../../cmake/modules/")
find_package(FlatBuffers REQUIRED)

set(FLAT_BUFFER_PATH ${CMAKE_CURRENT_LIST_DIR}/flatbuffer)
set(FLATBUFFER_INCLUDE_DIR "${FLAT_BUFFER_PATH}/includes")
set(FLATBUFFER_SCHEMA_DIR "${FLAT_BUFFER_PATH}/schemas")

set(FLAT_BUFFER_SCHEMAS
        ${FLATBUFFER_SCHEMA_DIR}/arena/ArenaServerAuth.fbs
        ${FLATBUFFER_SCHEMA_DIR}/arena/FightingPitEncounter.fbs
        ${FLATBUFFER_SCHEMA_DIR}/ClientCom.fbs
        ${FLATBUFFER_SCHEMA_DIR}/Notifications.fbs
        ${FLATBUFFER_SCHEMA_DIR}/WSAction.fbs)

if (TARGET generateFbs)
else()
    message("Flatbuffer generation ${FLATBUFFER_INCLUDE_DIR}")
    build_flatbuffers("${FLAT_BUFFER_SCHEMAS}"
        "${FLATBUFFER_SCHEMA_DIR}"
        generateFbs
        ""
        "${FLATBUFFER_INCLUDE_DIR}"
        ""
        "")
endif()

set(COMMON_SRCS
        ${CMAKE_CURRENT_LIST_DIR}/flatbuffer/includes/ClientCom_generated.h
        ${CMAKE_CURRENT_LIST_DIR}/flatbuffer/includes/Notifications_generated.h
        ${CMAKE_CURRENT_LIST_DIR}/flatbuffer/includes/WSAction_generated.h
        ${CMAKE_CURRENT_LIST_DIR}/src/RandomGenerator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ServiceContextBase.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/RandomGenerator.hh
        ${CMAKE_CURRENT_LIST_DIR}/include/BoundaryMap.hh
        ${CMAKE_CURRENT_LIST_DIR}/include/ServiceContextBase.hh
        ${CMAKE_CURRENT_LIST_DIR}/include/algorithm/algorithm.hh
        ${CMAKE_CURRENT_LIST_DIR}/include/Key.hh)
set(COMMON_INCLUDE_DIR
        ${CMAKE_CURRENT_LIST_DIR}/include)

include_directories(flatbuffer/includes)

#add_subdirectory(test)
