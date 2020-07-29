# configure a header file to pass some of the CMake settings
# to the source code
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/include/context/Versioner.hh.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/context/Versioner.hh"
)

add_library(common_dispatcher
        ${CMAKE_CURRENT_SOURCE_DIR}/include/core/SimpleProxy.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/core/Dispatcher.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/core/DispatcherConnectionManager.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/context/StartupDispatcherCtx.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/context/StartupDispatcherCtx.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/SimpleProxy.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/Dispatcher.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/DispatcherConnectionManager.cpp)

target_include_directories(common_dispatcher
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

set_target_properties(common_dispatcher PROPERTIES
        CXX_STANDARD 20
        CMAKE_CXX_COMPILER /usr/bin/g++)

target_link_libraries(common_dispatcher
        PUBLIC cppzmq fmt::fmt)
