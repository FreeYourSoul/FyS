cmake_minimum_required(VERSION 3.6...3.15)
if (${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else ()
    cmake_policy(VERSION 3.15)
endif ()

project(generic_dispatcher
        VERSION 1.0
        DESCRIPTION "Arena Dispatcher"
        LANGUAGES CXX)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#Build executable
add_executable(generic_dispatcher
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

target_compile_options(generic_dispatcher PRIVATE -O2 -fstack-protector-all -g)
target_compile_options(generic_dispatcher PRIVATE -Wall -Wpedantic -Wextra -Wno-deprecated-declarations)

target_link_libraries(generic_dispatcher
        PRIVATE common_dispatcher nlohmann_json::nlohmann_json)
target_compile_features(generic_dispatcher PRIVATE cxx_std_20)

message(STATUS Set Warnings flag : ${CMAKE_CXX_FLAGS})

install(TARGETS generic_dispatcher DESTINATION ${CMAKE_INSTALL_BINDIR})
