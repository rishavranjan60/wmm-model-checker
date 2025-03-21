cmake_minimum_required(VERSION 3.22)
project(WMM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include header files
include_directories(include)

# Add the engine library
add_library(engine src/tokenizer.cpp
        src/parser.cpp
        src/code.cpp
        src/program.cpp
        src/thread.cpp
        src/defs.cpp)

# Compiler warnings
target_compile_options(engine PRIVATE -Wall -Wextra -Wpedantic)

# Add the executable
add_executable(executor main.cpp)

# Link engine library with the executable
target_link_libraries(executor PRIVATE engine)

# Option to enable tests
option(ENABLE_TESTS "Enable tests build" ON)

# Add tests subdirectory if enabled
if (ENABLE_TESTS)
    add_subdirectory(tests)
endif ()
