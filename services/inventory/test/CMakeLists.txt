cmake_minimum_required(VERSION 3.6...3.15)
if (${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else ()
    cmake_policy(VERSION 3.15)
endif ()

project(inventory_test CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

find_package(cppzmq REQUIRED)
find_package(Catch2 REQUIRED)
find_package(fmt REQUIRED)
find_package(FSeam)
include(Catch)

include(${CMAKE_CURRENT_LIST_DIR}/../../common/CMakeLists.txt)

enable_testing()
include_directories(
        ${COMMON_INCLUDES_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/../include)

add_executable(inventory_test
        ${CMAKE_CURRENT_LIST_DIR}/catchMain.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ExchangeRoomTestCase.cpp
        )

target_compile_definitions(inventory_test PUBLIC -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
target_link_libraries(inventory_test cppzmq fmt::fmt dl cml)
