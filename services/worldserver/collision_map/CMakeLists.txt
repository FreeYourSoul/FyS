project(collision_map
        VERSION 1.0
        DESCRIPTION "FyS Collision Map format usage"
        LANGUAGES CXX)

add_library(collision_map STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/collision_map_converter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/collision_map_converter.hh
        )

target_link_libraries(collision_map PRIVATE
        fys::common_service dl)

add_library(fys::collision_map ALIAS collision_map)

if (BUILD_SERVICE_CLI_TMX_MAP_CONVERTER)

    add_executable(collision_map_converter ${CMAKE_CURRENT_SOURCE_DIR}/cli/cml_main.cpp)

    target_link_libraries(collision_map_converter PRIVATE
            fys::common_service
            fys::collision_map
            dl ${TMXLITE_LIBRARIES})

endif ()