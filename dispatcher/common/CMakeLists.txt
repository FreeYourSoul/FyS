cmake_minimum_required( VERSION 3.6 )

# The version number.
set (VERSION_MAJOR 1)
set (VERSION_MINOR 0)

set(COMMON_DIR ${PROJECT_SOURCE_DIR}/../common)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
        "${COMMON_DIR}/../common/include/context/Versioner.hh.in"
        "${COMMON_DIR}/../common/include/context/Versioner.hh"
)

include_directories(
        ${COMMON_DIR}/include/context
        ${COMMON_DIR}/include/core
)

set(COMMON_SRC
        ${COMMON_DIR}/include/core/Dispatcher.hh
        ${COMMON_DIR}/include/core/DispatcherConnectionManager.hh
        ${COMMON_DIR}/include/context/StartupDispatcherCtx.hh
        ${COMMON_DIR}/src/main.cpp
        ${COMMON_DIR}/src/context/StartupDispatcherCtx.cpp
        ${COMMON_DIR}/src/core/Dispatcher.cpp
        ${COMMON_DIR}/src/core/DispatcherConnectionManager.cpp)

