cmake_minimum_required(VERSION 3.15)
project(Btree C)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES main.c btree.c util.c util.h)

add_executable(history ${SOURCE_FILES})