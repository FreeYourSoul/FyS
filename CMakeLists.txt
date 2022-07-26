cmake_minimum_required(VERSION 3.6...3.15)
if (${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else ()
    cmake_policy(VERSION 3.15)
endif ()

project(FyS_Online CXX)

option(BUILD_CML "Build Cache utility library" ON)
option(BUILD_SERVICE_ARENA "Build the arena service server binary" ON)
option(BUILD_SERVICE_QUEST "Build the quest service server binary" ON)
option(BUILD_SERVICE_WORLD "Build the world service server binary" ON)
option(BUILD_SERVICE_INVENTORY "Build the inventory service server binary" OFF)
option(BUILD_DISP_CHAT "Build the Char dispatcher binary" OFF)
option(BUILD_DISP_GATEWAY "Build the Gateway dispatcher binary" ON)
option(BUILD_DISPATCHER "Build the Generic dispatcher binary" ON)
option(BUILD_SERVICE_CLI_TMX_MAP_CONVERTER "Build the command line tool to compile tmx files into collision map" OFF)

list(APPEND CMAKE_MODULE_PATH
        ${CMAKE_CURRENT_SOURCE_DIR}/misc/cmake
        ${CMAKE_CURRENT_SOURCE_DIR}/misc/cmake/modules)

list(APPEND PKG_CONFIG_PATH
        ${CMAKE_CURRENT_SOURCE_DIR}/misc/pkgconfig)

find_package(nlohmann_json REQUIRED)
find_package(cppzmq REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(FlatBuffers REQUIRED)
find_package(FSeam REQUIRED)
find_package(fil REQUIRED)

find_program(FLAT_COMPILER_FOUND flatc REQUIRED)
set(FLATBUFFERS_FLATC_EXECUTABLE ${FLAT_COMPILER_FOUND})

message(STATUS "flatc compiler found ${FLAT_COMPILER_FOUND}")

if (BUILD_SERVICE_ARENA)
    message(STATUS "Import Arena Service required package")
endif()
if (BUILD_SERVICE_WORLD)
    message(STATUS "Import World Service required package")
    find_package(Lua51)
    find_package(Lua)
    find_package(tmxlite REQUIRED)
endif()
if (BUILD_SERVICE_CLI_TMX_MAP_CONVERTER)
    message(STATUS "Import TMX Converter required package")
    find_package(tmxlite REQUIRED)
endif()
if (BUILD_TESTING)
    find_package(FSeam REQUIRED)
    find_package(Catch2 3 REQUIRED)
    enable_testing()
endif()

include(CMakeRC)

find_program(CCACHE_FOUND ccache)
if (CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif (CCACHE_FOUND)

if (BUILD_CML)
    add_subdirectory(scripting_cache)
    add_subdirectory(scripting_cache/cml)
endif()

add_subdirectory(dispatcher)
add_subdirectory(services)
