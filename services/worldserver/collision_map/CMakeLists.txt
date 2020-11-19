project(collision_map
        VERSION 1.0
        DESCRIPTION "FyS Collision Map format usage"
        LANGUAGES CXX)

add_library(collision_map STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/collision_map_converter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/collision_map_converter.hh
        )

target_link_libraries(collision_map PUBLIC
        fys::common_service dl ${TMXLITE_LIBRARIES})
target_include_directories(collision_map PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(fys::collision_map ALIAS collision_map)
target_compile_features(collision_map PRIVATE cxx_std_20)

if (BUILD_SERVICE_CLI_TMX_MAP_CONVERTER)

    add_executable(collision_map_converter ${CMAKE_CURRENT_SOURCE_DIR}/cli/cml_main.cpp)

    target_link_libraries(collision_map_converter PRIVATE
            fys::collision_map)
    target_compile_features(collision_map_converter PRIVATE cxx_std_20)

endif ()