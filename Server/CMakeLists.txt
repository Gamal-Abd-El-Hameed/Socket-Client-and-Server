cmake_minimum_required(VERSION 3.22)
project(Server)

set(CMAKE_CXX_STANDARD 14)

add_executable(Server main.cpp Server.cpp Server.h)
target_link_libraries(Server ws2_32)