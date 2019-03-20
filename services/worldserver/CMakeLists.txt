cmake_minimum_required( VERSION 3.6 )
project(worldserver_service CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

add_executable(worldserver_service
        ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp)

target_compile_options(worldserver_service PRIVATE -O2 -fstack-protector-all -g)
target_compile_options(worldserver_service PRIVATE -Wall -Wpedantic -Wextra -Wno-deprecated-declarations)

message( Compile worldserver_service : STATUS Set Warnings flag : ${CMAKE_CXX_FLAGS})
