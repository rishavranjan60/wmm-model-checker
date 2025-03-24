add_executable(test)
target_compile_features(test PUBLIC cxx_std_20)  # ✅ Enforce C++20

# add subdirectories: 
add_subdirectory(tokenizer)
add_subdirectory(parser)
add_subdirectory(modelchecker)
add_subdirectory(release_acquire)
add_subdirectory(pso)
add_subdirectory(sc)

target_link_libraries(test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(test PRIVATE engine)

