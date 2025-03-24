cmake_minimum_required(VERSION 3.14)
project(wmm-model-checker)

# ✅ Enforce C++20 globally
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# ✅ Catch2 for testing
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.2
)
FetchContent_MakeAvailable(Catch2)

# Include headers
include_directories(include)

# ✅ Build engine library with C++20
add_library(engine
    src/tokenizer.cpp
    src/parser.cpp
    src/code.cpp
    src/program.cpp
    src/thread.cpp
    src/defs.cpp
)
target_compile_features(engine PUBLIC cxx_std_20)
target_compile_options(engine PRIVATE -Wall -Wextra -Wpedantic)

# ✅ Build main executable with C++20
add_executable(executor main.cpp)
target_compile_features(executor PUBLIC cxx_std_20)
target_link_libraries(executor PRIVATE engine)

# ✅ Optionally build tests
option(ENABLE_TESTS "Enable tests build" ON)
if (ENABLE_TESTS)
    add_subdirectory(tests)
endif ()
