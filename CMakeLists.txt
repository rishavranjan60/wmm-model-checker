cmake_minimum_required(VERSION 3.22)
project(WMM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_library(engine src/tokenizer.cpp
        src/parser.cpp
        src/code.cpp
        src/program.cpp
        src/thread.cpp
        src/defs.cpp)

target_compile_options(engine PRIVATE -Wall -Wextra -Wpedantic)

add_executable(executor main.cpp)
target_link_libraries(executor PRIVATE engine)

option(ENABLE_TESTS "Enable tests build" ON)

if (ENABLE_TESTS)
    add_subdirectory(tests)
endif ()