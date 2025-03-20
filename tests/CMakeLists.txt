set(CMAKE_MODULE_PATH  "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Catch REQUIRED)

add_executable(test)

add_subdirectory(tokenizer)
add_subdirectory(parser)
add_subdirectory(release_acquire)
add_subdirectory(tso)
add_subdirectory(sc)

target_link_libraries(test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(test PRIVATE engine)
