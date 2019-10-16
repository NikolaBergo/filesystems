cmake_minimum_required(VERSION 3.14)
project(utf8)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(utf8
        main.cpp
        my_utf8.cpp
        my_utf8.h)
