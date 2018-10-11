cmake_minimum_required(VERSION 3.9.2)

project(chip8script)
add_executable(chip8script "main.cpp" "parser.hpp" "variable.hpp" "types.hpp")
target_compile_features(chip8script PRIVATE cxx_std_17)