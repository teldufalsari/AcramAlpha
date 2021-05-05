cmake_minimum_required(VERSION 3.0)
project (acram)

set(CMAKE_VERBOSE_MAKEFILE True)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set (CMAKE_CXX_FLAGS "-Wall -Wextra -Wnarrowing -Wfloat-equal -Wundef -Wshadow -Wunreachable-code -Wpointer-arith -Wcast-align -Wwrite-strings -Wswitch-default -Wswitch-enum -Winit-self -Wcast-qual -O2")

set(SOURCE common.cpp common.hpp expr_tree.cpp expr_tree.hpp parser.cpp parser.hpp texio.cpp texio.hpp main.cpp lib/vector.h)

add_executable(acram ${SOURCE})