cmake_minimum_required(VERSION 3.5)
project(test)

set(CMAKE_CXX_STANDARD 14)
set(GCC_COMPILE_FLAGS "-pthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COMPILE_FLAGS}")

add_executable(a.out user.c mysyscall.h)