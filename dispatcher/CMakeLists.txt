cmake_minimum_required( VERSION 3.6 )

set_property(TARGET PROPERTY CXX_STANDARD 20)

if (NOT RELATIVE_TO_COMMON)
    set(RELATIVE_TO_COMMON /common/)
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}${RELATIVE_TO_COMMON}cmake)
endif ()

add_subdirectory(inventoryexch_dispatcher)
add_subdirectory(inventoryusage_dispatcher)
add_subdirectory(inventorygrab_dispatcher)
add_subdirectory(quest_dispatcher)
add_subdirectory(chat_dispatcher)
add_subdirectory(worldserver_dispatcher)
add_subdirectory(arena_dispatcher)
add_subdirectory(proxy_cluster_dispatcher)
