#!/bin/bash -ex

pushd sdk-installer/
cp docker_start /usr/bin/
#wget gadget.exe
mkdir output
HOME="output" ./release.sh 1.0 master

