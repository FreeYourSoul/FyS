cmake_minimum_required(VERSION 3.6...3.15)
if (${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else ()
    cmake_policy(VERSION 3.15)
endif ()

project(cml_test CXX)

find_package(Catch2 REQUIRED)
include(Catch)

enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(cml_test
        ${CMAKE_CURRENT_SOURCE_DIR}/catch_main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/basic_cml_test_case.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cml_copy_test_case.cpp
        )
set(CMAKE_BUILD_TYPE Debug)
target_link_libraries(cml_test PUBLIC fys::cml Catch2::Catch2 stdc++fs)
target_compile_features(cml_test PRIVATE cxx_std_23)
catch_discover_tests(cml_test)
