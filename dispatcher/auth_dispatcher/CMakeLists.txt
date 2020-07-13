cmake_minimum_required( VERSION 3.6...3.15 )
if(${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.15)
endif()

project(auth_dispatcher
        VERSION 1.0
        DESCRIPTION "Auth Dispatcher"
        LANGUAGES CXX)

find_package(nlohmann_json REQUIRED)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}${RELATIVE_TO_COMMON}cmake)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

#Build executable
add_executable(auth_dispatcher
        ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp)

target_compile_options(auth_dispatcher PRIVATE -O2 -fstack-protector-all -g)
target_compile_options(auth_dispatcher PRIVATE -Wall -Wpedantic -Wextra -Wno-deprecated-declarations)

target_link_libraries(auth_dispatcher
        PRIVATE common_dispatcher nlohmann_json::nlohmann_json)

set_target_properties(auth_dispatcher PROPERTIES
        CXX_STANDARD 17
        CMAKE_CXX_COMPILER /usr/bin/g++)

message( STATUS Set Warnings flag : ${CMAKE_CXX_FLAGS})