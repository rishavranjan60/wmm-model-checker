cmake_minimum_required(VERSION 3.10)
project(pso_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(test_pso test_pso.cpp)
target_link_libraries(test_pso engine)  # Replace 'engine' with actual library name if needed


