cmake_minimum_required(VERSION 3.6...3.15)
if (${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else ()
    cmake_policy(VERSION 3.15)
endif ()

project(inventory_service
        VERSION 1.0
        DESCRIPTION "Arena Service Server"
        LANGUAGES CXX)

include(${CMAKE_CURRENT_LIST_DIR}/../common/CMakeLists.txt)

find_package(nlohmann_json REQUIRED)
find_package(cppzmq REQUIRED)
find_package(fmt REQUIRED)

find_program(CCACHE_FOUND ccache)
if (CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif (CCACHE_FOUND)

add_executable(inventory_service
        ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/InventoryServerContext.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/InventoryServerService.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/InventoryServerService.hh
        ${CMAKE_CURRENT_LIST_DIR}/include/InventoryServerContext.hh)

target_link_libraries(inventory_service PRIVATE
        common_service
        cppzmq
        nlohmann_json::nlohmann_json
        fmt::fmt dl cml
        ${Flatbuffers_LIBRARIES})

set_target_properties(inventory_service PROPERTIES
        CXX_STANDARD 17
        CMAKE_CXX_COMPILER /usr/bin/g++)

set_property(TARGET inventory_service PROPERTY INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/../../scripting_cache/cml/include
        ${fmt_INCLUDE_DIRS}
        ${FLATBUFFER_INCLUDE_DIR})

include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if (result)
    set_target_properties(inventory_service PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif ()

target_compile_options(inventory_service PRIVATE -O2 -fstack-protector-all -g -lfmt)
target_compile_options(inventory_service PRIVATE -Wall -Wpedantic -Wextra -Wno-deprecated-declarations)
target_compile_definitions(inventory_service PUBLIC CHAISCRIPT_NO_THREADS -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
