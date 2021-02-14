cmake_minimum_required(VERSION 3.17)
project(jeopardy_trainer)

set(CMAKE_CXX_STANDARD 14)

add_executable(jeopardy_trainer main.cpp Clue.h Clue.cpp)