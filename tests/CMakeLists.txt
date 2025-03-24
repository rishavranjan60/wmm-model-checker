add_executable(test)

# Add subdirectories for test modules
add_subdirectory(tokenizer)
add_subdirectory(parser)
add_subdirectory(modelchecker)
add_subdirectory(release_acquire)
add_subdirectory(pso)
add_subdirectory(sc)

target_link_libraries(test PRIVATE Catch2::Catch2WithMain engine)
