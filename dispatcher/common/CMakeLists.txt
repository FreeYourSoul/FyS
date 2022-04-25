# configure a header file to pass some of the CMake settings
# to the source code
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/include/context/Versioner.hh.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/context/Versioner.hh"
)

add_library(common_dispatcher
        ${CMAKE_CURRENT_SOURCE_DIR}/include/core/simple_proxy.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/core/dispatcher.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/core/dispatcher_connection_manager.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/context/startup_dispatcher_ctx.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/context/startup_dispatcher_ctx.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/simple_proxy.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/dispatcher.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/dispatcher_connection_manager.cpp)

target_include_directories(common_dispatcher PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/context
        ${CMAKE_CURRENT_SOURCE_DIR}/include/core
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

set_target_properties(common_dispatcher PROPERTIES
        CXX_STANDARD 20)

target_link_libraries(common_dispatcher
        PUBLIC spdlog::spdlog cppzmq fmt::fmt)
