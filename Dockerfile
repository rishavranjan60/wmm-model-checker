FROM ubuntu:latest
LABEL authors="mishok2503"

RUN apt-get update && apt-get -y --no-install-recommends install \
    build-essential \
    cmake \
    make \
    git \
    ca-certificates

RUN git config --global http.sslVerify false

COPY include/ srcs/include/
COPY src/ srcs/src/
COPY main.cpp srcs/
COPY CMakeLists.txt srcs/
COPY tests/ srcs/tests/

RUN mkdir build && cd build && \
    cmake ../srcs -DENABLE_TESTS=ON && \
    make && ctest --output-on-failure

COPY examples/ /build/
COPY tests/tso/programs /build/tests/tso/programs