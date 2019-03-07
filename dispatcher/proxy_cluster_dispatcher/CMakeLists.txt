cmake_minimum_required( VERSION 3.6 )

project(proxy_cluster_dispatcher)

#find cppzmq wrapper, installed by make of cppzmq
find_package(cppzmq)

include_directories(BEFORE /usr/include/tclap)

add_executable(proxy_cluster_dispatcher
        proxy.cpp
        )
target_link_libraries(proxy_cluster_dispatcher cppzmq)
