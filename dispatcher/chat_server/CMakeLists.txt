project(chatserver
        VERSION 1.0
        DESCRIPTION "Chat Server"
        LANGUAGES CXX)

add_executable(chatserver
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/chat_engine.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/chat_engine.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/chatserver_context.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/chatserver_context.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/chat_room.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/chat_room.hh
        )

target_link_libraries(chatserver
        nlohmann_json::nlohmann_json
        cppzmq
        lua
        fys::common_service
        fmt::fmt dl fys::cml
        ${Flatbuffers_LIBRARIES})
target_include_directories(chatserver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS chatserver DESTINATION ${CMAKE_INSTALL_BINDIR})
