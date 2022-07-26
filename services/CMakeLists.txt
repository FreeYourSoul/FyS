add_subdirectory(common)

# FSeam is not suitable to work with Catch2 version 3
set(FSEAM_USE_CATCH2 OFF)

if (BUILD_SERVICE_ARENA)
    add_subdirectory(arena)
    if (BUILD_TESTING)
        add_subdirectory(arena/test)
    endif ()
endif ()

if (BUILD_SERVICE_INVENTORY)
    add_subdirectory(inventory)
    if (BUILD_TESTING)
        add_subdirectory(inventory/test)
    endif ()
endif ()

if (BUILD_SERVICE_QUEST)
    add_subdirectory(quest)
    if (BUILD_TESTING)
        add_subdirectory(quest/test)
    endif ()
endif ()

if (BUILD_SERVICE_WORLD)
    add_subdirectory(worldserver/collision_map)
    add_subdirectory(worldserver)
    if (BUILD_TESTING)
        add_subdirectory(worldserver/test)
    endif ()
endif ()
