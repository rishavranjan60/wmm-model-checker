cmake_minimum_required(VERSION 3.10)
project(pso_tests)  # Changed to avoid conflict with executable name

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set path for custom CMake modules (if you're using a custom Catch2.cmake)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake")

# Find Catch2
find_package(Catch REQUIRED)

# Add the executable
add_executable(pso_test pso_test.cpp)

# Link with Catch2 and engine
target_link_libraries(pso_test PRIVATE Catch2::Catch2WithMain engine)

# Include testing support
include(CTest)
include(Catch)

# Automatically discover tests using Catch2
catch_discover_tests(pso_test)
