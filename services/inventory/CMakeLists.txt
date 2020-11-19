project(inventory_service
        VERSION 1.0
        DESCRIPTION "Arena Service Server"
        LANGUAGES CXX)

add_executable(inventory_service
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/inventory_server_context.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/inventory_server_service.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/exchange/exchange_room.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/exchange/exchange_manager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/item_manager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/flatbuffer_generator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/flatbuffer_generator.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/item_manager.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/exchange/exchange_manager.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/exchange/exchange_room.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/inventory_server_service.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/inventory_server_context.hh)

target_link_libraries(inventory_service PRIVATE
        fys::common_service
        dl fys::cml)
target_compile_features(inventory_service PRIVATE cxx_std_20)

target_include_directories(inventory_service PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if (result)
    set_target_properties(inventory_service PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif ()

target_compile_options(inventory_service PRIVATE -O2 -fstack-protector-all -g -lfmt)
target_compile_options(inventory_service PRIVATE -Wall -Wpedantic -Wextra -Wno-deprecated-declarations)
target_compile_definitions(inventory_service PUBLIC CHAISCRIPT_NO_THREADS -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)

install(TARGETS inventory_service DESTINATION ${CMAKE_INSTALL_BINDIR})
