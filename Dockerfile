FROM ubuntu:latest
LABEL authors="mishok2503"

RUN apt-get update && apt-get -y --no-install-recommends install \
    build-essential \
    cmake \
    make

COPY include/ srcs/include/
COPY src/ srcs/src/
COPY main.cpp srcs/
COPY CMakeLists.txt srcs/
RUN mkdir build && cd build && cmake ../srcs && make
COPY examples/ /