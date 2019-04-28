cmake_minimum_required( VERSION 3.6 )
project(worldserver_service CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/modules/")

find_package(FlatBuffers    REQUIRED)
find_package(TMXLITE        REQUIRED)
find_package(nlohmann_json  REQUIRED)
find_package(cppzmq         REQUIRED)

set(FLAT_BUFFER_SRCS ${CMAKE_CURRENT_LIST_DIR}/../flatbuffer)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

include_directories(
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/../common/include
        ${TMXLITE_HEADERS})

add_executable(worldserver_service
        ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../common/src/ServiceContextBase.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../common/include/ServiceContextBase.hh
        ${CMAKE_CURRENT_LIST_DIR}/../common/include/Key.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/engine/WorldServerEngine.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/WorldServerEngine.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/engine/PlayersData.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/PlayersData.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/engine/Map.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/Map.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/ConnectionHandler.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/ConnectionHandler.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/WorldServerService.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/WorldServerService.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/WorldServerContext.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/WorldServerContext.hh)

build_flatbuffers(
        "${FLAT_BUFFER_SRCS}/schemas/Move.fbs"
        "${FLAT_BUFFER_SRCS}/schemas"
        generateFbs
        ""
        "${FLAT_BUFFER_SRCS}/includes"
        ""
        "")

target_link_libraries(worldserver_service cppzmq ${TMXLITE_LIBRARIES} ${Flatbuffers_LIBRARIES})

target_compile_options(worldserver_service PRIVATE -O2 -fstack-protector-all -g)
target_compile_options(worldserver_service PRIVATE -Wall -Wpedantic -Wextra -Wno-deprecated-declarations)
