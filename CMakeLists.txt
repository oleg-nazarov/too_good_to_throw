cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)

project(WebChat)

add_executable(web_chat main.cpp)

find_path(UWEBSOCKETS_INCLUDE_DIRS uwebsockets)
find_library(USOCKETS_LIB uSockets)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(ZLIB REQUIRED)

target_include_directories(web_chat PRIVATE ${UWEBSOCKETS_INCLUDE_DIRS})
target_link_libraries(web_chat PRIVATE
    nlohmann_json::nlohmann_json
    ${USOCKETS_LIB}
    ZLIB::ZLIB)
