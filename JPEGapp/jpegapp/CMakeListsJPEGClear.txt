cmake_minimum_required(VERSION 3.17)
project(jpegapp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS -static)

include_directories(JPEG)

add_executable(jpegclear
        JPEG/ReadFile.cpp
        JPEG/ReadFile.h
        JPEG/JPEG.cpp
        JPEG/JPEG.h
        JPEG/Huffman.cpp
        JPEG/Huffman.h
        jpegclear.cpp)