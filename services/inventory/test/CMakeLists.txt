cmake_minimum_required(VERSION 3.6...3.15)
if (${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else ()
    cmake_policy(VERSION 3.15)
endif ()

project(inventory_test CXX)

find_package(cppzmq REQUIRED)
find_package(Catch2 3 REQUIRED)
find_package(fmt REQUIRED)
find_package(FSeam)
include(Catch)

enable_testing()
include_directories(
        ${COMMON_INCLUDES_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(inventory_test
        ${CMAKE_CURRENT_SOURCE_DIR}/exchange_room_test_case.cpp

        # Sources
        ${COMMON_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/inventory_server_context.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/inventory_server_service.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/exchange/exchange_room.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/exchange/exchange_manager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/item_manager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/flatbuffer_generator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/flatbuffer_generator.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/item_manager.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/exchange/exchange_manager.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/exchange/exchange_room.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/inventory_server_service.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/inventory_server_context.hh
        )

target_compile_definitions(inventory_test PUBLIC -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
target_link_libraries(inventory_test Catch2::Catch2WithMain cppzmq fmt::fmt dl fys::cml)
target_compile_features(inventory_test PRIVATE cxx_std_20)
