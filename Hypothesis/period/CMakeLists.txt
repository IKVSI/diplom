cmake_minimum_required(VERSION 3.17)
project(period)

set(CMAKE_CXX_STANDARD 14)

add_executable(period main.cpp Function.cpp Function.h)
target_link_libraries(period -static)