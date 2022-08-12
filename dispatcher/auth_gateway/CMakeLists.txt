project(auth_gateway
        VERSION 1.0
        DESCRIPTION "Auth Dispatcher"
        LANGUAGES CXX)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#Build executable
add_executable(auth_gateway
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

target_compile_options(auth_gateway PRIVATE -Wall -Wpedantic -Wextra -Wno-deprecated-declarations)

target_link_libraries(auth_gateway
        PRIVATE common_dispatcher nlohmann_json::nlohmann_json)
target_compile_features(auth_gateway PRIVATE cxx_std_23)

message(STATUS Set Warnings flag : ${CMAKE_CXX_FLAGS})
install(TARGETS auth_gateway DESTINATION ${CMAKE_INSTALL_BINDIR})
