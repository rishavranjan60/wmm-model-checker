FROM ubuntu:latest
LABEL authors="mishok2503"

# Install necessary dependencies
RUN apt-get update && apt-get -y --no-install-recommends install \
    build-essential \
    cmake \
    make \
<<<<<<< HEAD
    git \
    ca-certificates

COPY include/ srcs/include/
COPY src/ srcs/src/
COPY main.cpp srcs/
COPY CMakeLists.txt srcs/
COPY tests/ srcs/tests/

RUN mkdir build && cd build && \
    cmake ../srcs -DENABLE_TESTS=ON && \
    make && ctest --output-on-failure

COPY examples/ /build/
=======
    g++ \
    python3 \
    python3-pip

# Set the working directory
WORKDIR /srcs

# Copy source files
COPY include/ include/
COPY src/ src/
COPY main.cpp .
COPY CMakeLists.txt .

# Create build directory and compile
WORKDIR /build
RUN cmake /srcs && make

# Copy examples and tests
COPY examples/ /build/examples/
COPY tests/ /build/tests/

# Set the default command (optional, depending on your project)
CMD ["./tests/pso/test_pso"]
>>>>>>> 86d3c0e (test pso)
