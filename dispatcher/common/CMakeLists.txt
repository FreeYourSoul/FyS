cmake_minimum_required(VERSION 3.6...3.15)
if (${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else ()
    cmake_policy(VERSION 3.15)
endif ()

set(CMAKE_VERBOSE_MAKEFILE ON)

# The version number.
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)

set(COMMON_DIR ${CMAKE_SOURCE_DIR}${RELATIVE_TO_COMMON})

find_package(cppzmq REQUIRED)
find_package(TCLAP REQUIRED)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file(
        "${COMMON_DIR}/include/context/Versioner.hh.in"
        "${COMMON_DIR}/include/context/Versioner.hh"
)

find_program(CCACHE_FOUND ccache)
if (CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif (CCACHE_FOUND)

add_library(common_dispatcher
        ${COMMON_DIR}/include/core/SimpleProxy.hh
        ${COMMON_DIR}/include/core/Dispatcher.hh
        ${COMMON_DIR}/include/core/DispatcherConnectionManager.hh
        ${COMMON_DIR}/include/context/StartupDispatcherCtx.hh
        ${COMMON_DIR}/src/context/StartupDispatcherCtx.cpp
        ${COMMON_DIR}/src/core/SimpleProxy.cpp
        ${COMMON_DIR}/src/core/Dispatcher.cpp
        ${COMMON_DIR}/src/core/DispatcherConnectionManager.cpp)

target_include_directories(common_dispatcher
        PUBLIC
        ${COMMON_DIR}/include
        )

set_target_properties(common_dispatcher PROPERTIES
        CXX_STANDARD 20
        CMAKE_CXX_COMPILER /usr/bin/g++)

target_link_libraries(common_dispatcher
        PUBLIC cppzmq)
