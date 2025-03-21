# Corrected tests/pso/CMakeLists.txt
set(CMAKE_MODULE_PATH  "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Catch REQUIRED)

# Add test executable (as a library or separate executable)
add_executable(test)

# Add the subdirectories
add_subdirectory(tokenizer)
add_subdirectory(parser)
add_subdirectory(modelchecker)
add_subdirectory(release_acquire)
add_subdirectory(tso)
add_subdirectory(sc)
add_subdirectory(pso)

# Link test with Catch2 and engine
target_link_libraries(test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(test PRIVATE engine)

# Ensure test_pso is added to the final build
add_dependencies(test test_pso)  # This line ensures test_pso is built with the rest of the tests
