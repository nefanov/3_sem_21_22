cmake_minimum_required(VERSION 3.10)
project(solution)

set(CMAKE_C_STANDARD 11)

add_executable(solution
        src/main_src.c
        src/string_src.c
        src/pipe_src.c
        include/main_src.h
        include/string_src.h
        include/pipe_src.h)
include_directories(include)
