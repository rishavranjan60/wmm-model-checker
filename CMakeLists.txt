cmake_minimum_required(VERSION 3.26)
project(WMM)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_library(engine src/tokenizer.cpp
        src/parser.cpp
        src/code.cpp)

add_executable(executor main.cpp)
target_link_libraries(executor PRIVATE engine)

# TODO: add if
add_subdirectory(tests)