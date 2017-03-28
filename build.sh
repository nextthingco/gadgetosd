#!/bin/bash

DOCKER_REGISTRY=ntc-registry.githost.io
DOCKER_IMAGE=ntc-registry.githost.io/nextthingco/gadgetosd:latest

if [[ -n "$(which docker)" ]]; then
    echo "docker found: $(which docker)"
    echo "trying a docker build"

    if ! docker -D pull $DOCKER_IMAGE; then
        docker login $DOCKER_REGISTRY
        docker -D pull $DOCKER_IMAGE || exit
    fi

    docker run -v $PWD:/work -w /work -e BUILD_DIR=build_docker --rm -it $DOCKER_IMAGE ./build_cmake.sh
fi
