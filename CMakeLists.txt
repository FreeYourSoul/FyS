cmake_minimum_required(VERSION 3.6...3.15)
if (${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else ()
    cmake_policy(VERSION 3.15)
endif ()

project(FyS_Online CXX)

find_package(nlohmann_json REQUIRED)
find_package(fmt REQUIRED)

list(APPEND CMAKE_MODULE_PATH
        ${CMAKE_CURRENT_LIST_DIR}/cmake
        ${CMAKE_CURRENT_LIST_DIR}/cmake/modules)

find_package(nlohmann_json REQUIRED)
find_package(cppzmq REQUIRED)
find_package(Catch2 REQUIRED)
find_package(Lua REQUIRED)
find_package(fmt REQUIRED)
find_package(FlatBuffers REQUIRED)

include(CMakeRC)

find_program(CCACHE_FOUND ccache)
if (CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif (CCACHE_FOUND)

if (BUILD_TESTING)
    enable_testing()
endif ()

add_subdirectory(scripting_cache)
add_subdirectory(deps/FiL)
add_subdirectory(scripting_cache/cml)
add_subdirectory(dispatcher)
add_subdirectory(services)
