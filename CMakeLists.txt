cmake_minimum_required(VERSION 3.14)
project(wmm-model-checker)

# Enable testing by default
option(ENABLE_TESTS "Enable tests build" ON)

# Include FetchContent to auto-fetch Catch2
include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.2
)

FetchContent_MakeAvailable(Catch2)

# Include headers
include_directories(include)

# Build the core engine library
add_library(engine
    src/tokenizer.cpp
    src/parser.cpp
    src/code.cpp
    src/program.cpp
    src/thread.cpp
    src/defs.cpp
)

target_compile_options(engine PRIVATE -Wall -Wextra -Wpedantic)

# Build main executable
add_executable(executor main.cpp)
target_link_libraries(executor PRIVATE engine)

# Add tests if enabled
if (ENABLE_TESTS)
    add_subdirectory(tests)
endif()
