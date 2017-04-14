#!/bin/bash

win_build_dir="${PWD}"
src="${win_build_dir}/../"

pushd sdk-installer
mkdir build
pushd build

cmake ${src}
make

cp gadget.exe ../

popd

./release.sh 1.0 master
