# Tools and Dependencies

This project utilizes the following tools and dependencies:

## Build and Development Tools

- **CMake (v3.22+)** - Cross-platform build system that manages the build process in a compiler-independent manner. [Website](https://cmake.org/)

- **C++20** - The project is built using the C++20 standard, which provides features like concepts, ranges, and coroutines. [Standard](https://en.cppreference.com/w/cpp/20)

- **Docker** - Used for containerization and ensuring consistent build environments across different platforms. [Website](https://www.docker.com/)

- **clang-format** - Code formatting tool used to maintain consistent code style throughout the project. [Documentation](https://clang.llvm.org/docs/ClangFormat.html)

- **clang-tidy** - Static code analysis tool that identifies and fixes typical programming errors and enforces coding standards. [Documentation](https://clang.llvm.org/extra/clang-tidy/)

## Testing

- **Catch2 (v3.4.0)** - A modern C++ test framework used for unit and integration testing in this project. [GitHub](https://github.com/catchorg/Catch2)

- **CTest** - Testing tool distributed as a part of CMake, used to run the test suite. [Documentation](https://cmake.org/cmake/help/latest/manual/ctest.1.html)

## CI/CD

- **GitHub Actions** - CI/CD platform used for automating the build process, running tests, and checking code formatting. [Documentation](https://docs.github.com/en/actions)

## Build Scripts

- **build_debug.sh** - Shell script for building the project in debug mode with additional debugging information.

- **build_release.sh** - Shell script for building optimized release versions of the project without tests.

## Operating Systems Support

The project has been tested on:
- Ubuntu (latest) - Primary development and testing environment, used in Docker and CI. 