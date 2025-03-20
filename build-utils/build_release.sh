#!/usr/bin/env bash

mkdir "$1"
cmake . -D CMAKE_BUILD_TYPE=Release ENABLE_TESTS=OFF
cmake --build "$1"
