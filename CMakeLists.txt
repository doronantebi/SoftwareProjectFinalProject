cmake_minimum_required(VERSION 3.15)
project(SoftwareProjectFinalProject C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -Wall -Wextra -Werror -pedantic-errors")
add_executable(SoftwareProjectFinalProject main.c game.c game.h init.c init.h edit.c edit.h solve.c solve.h
        main_aux.c main_aux.h solver.c solver.h gurobi.h gurobi.c utilities.c utilities.h Parser.c Parser.h)