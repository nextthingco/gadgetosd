#!/bin/bash


BUILD_DIR="${BUILD_DIR:-build}"

mkdir "${BUILD_DIR}"
pushd "${BUILD_DIR}"
cmake ..
make
cpack
popd
