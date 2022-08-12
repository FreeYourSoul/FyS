project(agent
        VERSION 1.0
        DESCRIPTION "Agent Controller & library"
        LANGUAGES CXX)

# agent :: library =====

add_library(agent
        ${CMAKE_CURRENT_SOURCE_DIR}/agent/src/unit.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/agent/includes/unit.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/agent/src/agent.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/agent/includes/agent.hh
        )

target_link_libraries(agent PRIVATE
        spdlog::spdlog
        fmt::fmt
        fys::cml
        dl
        )
add_library(fys::agent ALIAS agent)
target_include_directories(agent PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/agent/includes)

# agent_controller :: binary =====

add_executable(agent_controller
        ${CMAKE_CURRENT_SOURCE_DIR}/agent_controller/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/agent_controller/src/controller.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/agent_controller/includes/controller.hh
        )
target_include_directories(agent_controller PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/agent_controller/includes)
target_link_libraries(agent_controller PRIVATE
        fys::agent
        spdlog::spdlog
        fmt::fmt
        )


