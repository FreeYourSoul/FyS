cmake_minimum_required(VERSION 3.6...3.15)
if (${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else ()
    cmake_policy(VERSION 3.15)
endif ()

cmrc_add_resource_library(arena_resources ALIAS resource::ar NAMESPACE resource
        scripts/arena/actions/action.chai
        scripts/arena/actions/damage/damage.chai
        scripts/arena/actions/zone_heal/zone_heal.chai
        scripts/arena/actions/zone_damage/zone_damage.chai
        scripts/arena/actions/heal/heal.chai)
