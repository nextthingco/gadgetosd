#!/bin/bash

OS=UNKNOWN
SUDO=""

INSTALL_DIR=/usr/share/gadget/templates/alpine

ALPINE_ROOTFS_URL=${ALPINE_ROOTFS_URL:-https://chiptainer_alpine.surge.sh/alpine_armhf_rootfs.tar.gz}

if [[ $EUID -ne 0 ]]; then
   export SUDO=sudo -i
fi

function get_os_type()
{
    case "$OSTYPE" in
        darwin*)  echo "Sorry OSX is not supportd yet."; exit 1 ;; 
        linux*)
            export OS=Linux
            export RELEASE_ID=$(awk -F= '/^ID=/ {print $2;}' /etc/os-release)
            export RELEASE_VERSION=$(awk -F= '/^VERSION_ID=/ {print $2;}' /etc/os-release)

            if [[ RELEASE_ID == "ubuntu" ]]; then
                INSTALL=apt-get install
                DOWNLOAD=wget -c
            fi
            ;;
        solaris*) echo "Sorry, Solaris is not supported yet."; exit 1 ;;
        bsd*)     echo "Sorry, BSD is not supported yet"; exit 1 ;;
        msys*)    echo "Sorry, WINDOWS is not supported yet"; exit 1 ;;
        *)        echo "Sorry, unknown operating system."; exit 1 ;;
     esac
}

function check_install()
{
    exe=$(whereis -b $1 |awk '{print $2}')
    if [[ -z "$exe" ]]; then
        echo "$1 is not found. Please install: $SUDO $INSTALL $1"
    else
        echo "found $exe"
    fi
}

echo -e "Installing Gadget\n"

get_os_type

echo -e "You are running $OS, $RELEASE_ID $RELEASE_VERSION\n"

check_install git
check_install docker

$SUDO mkdir -p "${INSTALL_DIR}"
$SUDO wget -c $ALPINE_ROOTFS_URL -O "${INSTALL_DIR}"/rootfs.tar.gz
$SUDO cp templates/alpine/blink-leds "${INSTALL_DIR}"/
$SUDO cp templates/alpine/Dockerfile "${INSTALL_DIR}"/

