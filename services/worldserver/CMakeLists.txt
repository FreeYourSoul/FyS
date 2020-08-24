project(worldserver_service
        VERSION 1.0
        DESCRIPTION "WorldServer Service Server"
        LANGUAGES CXX)

add_executable(worldserver_service
        ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/engine/player_data.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/player_data.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/engine/engine.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/engine.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/engine/collision_map.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/flatbuffer_generator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/flatbuffer_generator.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/engine/script_engine.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/script_engine.hh
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/collision_map.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/connection_handler.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/connection_handler.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/world_server_service.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/world_server_service.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/world_server_context.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/world_server_context.hh
        ${CMAKE_CURRENT_LIST_DIR}/src/engine/world_populator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/engine/world_populator.hh
        )

target_link_libraries(worldserver_service
        lua
        fys::common_service
        dl fys::cml
        ${TMXLITE_LIBRARIES} # to remove
        )

target_include_directories(worldserver_service PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${COMMON_INCLUDES_DIR}
        ${FLATBUFFER_INCLUDE_DIR}
        ${TMXLITE_HEADERS})

include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if (result)
    set_target_properties(arena_service PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif ()

target_compile_options(worldserver_service PRIVATE -O2 -fstack-protector-all -g)
target_compile_options(worldserver_service PRIVATE -Wall -Wpedantic -Wextra -Wno-deprecated-declarations)
