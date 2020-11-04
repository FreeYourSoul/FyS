add_subdirectory(common)

if (BUILD_SERVICE_ARENA)
    add_subdirectory(arena)
endif()

if (BUILD_SERVICE_INVENTORY)
    add_subdirectory(inventory)
endif()

if (BUILD_SERVICE_QUEST)
    add_subdirectory(quest)
endif()

if (BUILD_SERVICE_WORLD)
    add_subdirectory(worldserver)
endif()

if (BUILD_TESTING)
    find_package(FSeam REQUIRED)
    include(Catch REQUIRED)

    if (BUILD_SERVICE_ARENA)
        add_subdirectory(arena/test)
    endif()

    if (BUILD_SERVICE_WORLD)
        add_subdirectory(worldserver/test)
    endif()

    if (BUILD_SERVICE_INVENTORY)
        add_subdirectory(inventory/test)
    endif()

endif ()
