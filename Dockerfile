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
COPY tests/ tests/

# Create build directory and compile
WORKDIR /build
RUN cmake /srcs -DENABLE_TESTS=ON && make && ./tests/test

# Copy examples
COPY examples/ /build/examples/

# Default command (optional)
CMD ["./tests/test"]
