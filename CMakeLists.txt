cmake_minimum_required(VERSION 3.30)
project(SudokuSolver C)

set(CMAKE_C_STANDARD 23)

add_executable(SudokuSolver main.c sudoku.c solver.c utils.c)
