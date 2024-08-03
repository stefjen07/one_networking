cmake_minimum_required(VERSION 3.28)
project(one_networking)

set(CMAKE_CXX_STANDARD 17)

add_library(one_networking server/server.cpp client/client.cpp message_length.cpp)
add_executable(one_networking_server main_server.cpp)
add_executable(one_networking_client main_client.cpp)
target_link_libraries(one_networking_server one_networking)
target_link_libraries(one_networking_client one_networking)