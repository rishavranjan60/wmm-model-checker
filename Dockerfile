FROM ubuntu:latest
LABEL authors="mishok2503"

# Install necessary dependencies
RUN apt-get update && apt-get -y --no-install-recommends install \
    build-essential \
    cmake \
    make \
    git \
    ca-certificates \
    g++ \
    python3 \
    python3-pip

# Set working directory
WORKDIR /srcs

# Copy source files
COPY include/ include/
COPY src/ src/
COPY main.cpp .
COPY CMakeLists.txt .

# Create build directory and compile
WORKDIR /build
RUN cmake /srcs -DENABLE_TESTS=ON && make && ctest --output-on-failure

# Copy examples and tests
COPY examples/ /build/examples/
COPY tests/ /build/tests/

# Default command (optional)
CMD ["./tests/pso/test_pso"]
