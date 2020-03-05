cmake_minimum_required( VERSION 3.6 )

project(FyS_Online CXX)

set_property(TARGET PROPERTY CXX_STANDARD 20)

set(RELATIVE_TO_COMMON /dispatcher/common/)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}${RELATIVE_TO_COMMON}cmake)

add_subdirectory(scripting_cache/cml)
add_subdirectory(dispatcher)
add_subdirectory(services)
