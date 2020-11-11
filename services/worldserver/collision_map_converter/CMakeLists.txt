project(collision_map_converter
        VERSION 1.0
        DESCRIPTION "Convert TMX map to FyS CollisionMap"
        LANGUAGES CXX)

add_executable(collision_map_converter
        ${CMAKE_CURRENT_SOURCE_DIR}/cml_main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/collision_map_converter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/collision_map_converter.hh
        )

target_link_libraries(collision_map_converter PRIVATE
        fys::common_service
        dl ${TMXLITE_LIBRARIES}
        )