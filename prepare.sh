#! /usr/bin/env bash
set -e

echo "Compiling benchmark..."

mkdir -p build
cd build
make clean
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j 2 