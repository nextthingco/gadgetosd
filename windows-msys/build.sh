#!/bin/bash -ex

win_build_dir="${PWD}"
src="${win_build_dir}/.."

pushd gadget-shell/sdk-installer
mkdir build
pushd build

cmake ${src}
make

cp gadget.exe ${win_build_dir}/gadget-shell/sdk-installer
cp -a ${src}/templates ${win_build_dir}/gadget-shell/sdk-installer/
cp rootfs.tar.gz ${win_build_dir}/gadget-shell/sdk-installer/templates/alpine

popd

./release.sh 1.0 master

popd

mkdir win
mv /c/Windows/system32/config/systemprofile/gadget-shell-installer-1.0-64.7z.exe win/
