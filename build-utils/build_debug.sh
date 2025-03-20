#!/usr/bin/env bash

mkdir "$1"
cmake . -D CMAKE_BUILD_TYPE=Debug
cmake --build "$1"
