#!/bin/bash


BUILD_TYPE="${BUILD_TYPE:-Release}"
BUILD_DIR="${BUILD_DIR:-build}"

mkdir "${BUILD_DIR}"
pushd "${BUILD_DIR}"
cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ..
cmake --build .
cpack
lintian -I gadgetos-cli_0.0.0-1_amd64.deb
popd
