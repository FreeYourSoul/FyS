add_subdirectory(common)

if (BUILD_DISP_GATEWAY)
    add_subdirectory(auth_gateway)
endif ()
if (BUILD_DISP_CHAT)
    add_subdirectory(chat_server)
endif ()
if (BUILD_DISPATCHER)
    add_subdirectory(generic_dispatcher)
endif ()
