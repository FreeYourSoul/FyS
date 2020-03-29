cmake_minimum_required( VERSION 3.6...3.15 )
if(${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.15)
endif()

project(FyS_Online CXX)

set_property(TARGET PROPERTY CXX_STANDARD 20)

set(RELATIVE_TO_COMMON /dispatcher/common/)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}${RELATIVE_TO_COMMON}cmake)

add_subdirectory(scripting_cache/cml)
add_subdirectory(dispatcher)
add_subdirectory(services)
