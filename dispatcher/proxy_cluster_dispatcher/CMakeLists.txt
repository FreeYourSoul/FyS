cmake_minimum_required( VERSION 3.6...3.15 )
if(${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.15)
endif()

project(proxy_cluster_dispatcher)

#find cppzmq wrapper, installed by make of cppzmq
find_package(cppzmq)

include_directories(BEFORE /usr/include/tclap)

add_executable(proxy_cluster_dispatcher
        proxy.cpp
        )
target_link_libraries(proxy_cluster_dispatcher cppzmq)
