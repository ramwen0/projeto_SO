cmake_minimum_required(VERSION 3.26)
project(projeto1 C)

set(CMAKE_C_STANDARD 11)

include_directories(include)

add_executable(projeto1
        main.c
        queue.c
        inputs.c
        simulation.c)
