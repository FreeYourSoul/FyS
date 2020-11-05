cmake_minimum_required(VERSION 3.6...3.15)
if (${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else ()
    cmake_policy(VERSION 3.15)
endif ()

project(cml CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

find_package(nlohmann_json REQUIRED)
find_package(fmt)


add_library(cml
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Cml.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Cml.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/CmlKey.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/CmlScriptDownloader.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/CmlScriptDownloader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/CmlCopy.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/CmlCopy.cpp)
add_library(fys::cml ALIAS cml)
target_include_directories(cml PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(cml spdlog fmt::fmt dl stdc++fs)
target_compile_options(cml PRIVATE -Wall -Wpedantic -Wextra -Wno-deprecated-declarations)

add_subdirectory(test)
