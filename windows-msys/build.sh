#!/bin/bash -ex

win_build_dir="${PWD}"
src="${win_build_dir}/.."

pushd gadget-shell/sdk-installer
mkdir build
pushd build

cmake ${src}
make

#cp gadget.exe ${win_build_dir}/gadget-shell/sdk-installer
#cp -a ${src}/templates ${win_build_dir}/gadget-shell/sdk-installer/
#cp rootfs.tar.gz ${win_build_dir}/gadget-shell/sdk-installer/templates/alpine

cp gadget.exe /usr/bin/
mkdir -p /usr/share/gadget
cp -a ${src}/templates /usr/share/gadget/
cp rootfs.tar.gz /usr/share/gadget/alpine/
cp ${win_build_dir}/gadget-shell/sdk-installer/docker_start /usr/bin/
cp ${win_build_dir}/gadget-shell/sdk-installer/gadget_init_docker.sh /etc/profile.d/

popd

./release.sh 1.0 master

popd

rm -rf /usr/bin/gadget.exe
rm -rf /usr/bin/docker_start
rm -rf /usr/share/gadget
rm -rf /etc/profile.d/gadget_init*

mkdir ../win
cp /c/Windows/system32/config/systemprofile/gadget-shell-installer-1.0-64.7z.exe /c/
mv /c/Windows/system32/config/systemprofile/gadget-shell-installer-1.0-64.7z.exe ../win/
