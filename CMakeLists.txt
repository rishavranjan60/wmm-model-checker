cmake_minimum_required(VERSION 3.26)
project(WMM)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(WMM main.cpp  src/program.cpp)
