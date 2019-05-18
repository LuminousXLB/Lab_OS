cmake_minimum_required(VERSION 3.14)
project(DynamicMemory C)

set(CMAKE_C_STANDARD 11)

add_executable(DynamicMemory main.c lmem.c lmem.h)