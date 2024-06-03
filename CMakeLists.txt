cmake_minimum_required(VERSION 3.24)
project(ex2)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(ex2
        thread.cpp
        thread.h
        uthreads.cpp
        uthreads.h)
