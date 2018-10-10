cmake_minimum_required(VERSION 3.9.2)

project(cpp-snippets)
add_executable(cpp-snippets "main.cpp")
target_compile_features(cpp-snippets PRIVATE cxx_std_17)