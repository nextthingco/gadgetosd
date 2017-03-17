#!/bin/bash -x

OS=UNKNOWN
SUDO=""

INSTALL_DIR=/usr/local/share/gadget/templates/alpine

ALPINE_ROOTFS_URL=${ALPINE_ROOTFS_URL:-https://chiptainer_alpine.surge.sh/alpine_armhf_rootfs.tar.gz}

function check_root()
{
 if [[ $EUID -ne 0 ]]; then
    echo "This script needs super user right. Try: sudo $0"
    exit 1
 fi
}

function has()
{
    [[ x"$(which $1)" == x ]] && return 1;
    return 0;
}

function download()
{
    echo "Downloading $1"
    SRC=$1
    DST=${2:-${1##*/}}

    if has curl; then
        curl -o $DST $SRC
    elif has wget; then
        wget -c $SRC -O $DST 
    fi
}

function get_os_type()
{
    case "$OSTYPE" in
        darwin*)
            check_root
            export OS=Mac
            INSTALL=brew
            ;;
        linux*)
            check_root
            export OS=Linux
            export RELEASE_ID=$(awk -F= '/^ID=/ {print $2;}' /etc/os-release)
            export RELEASE_VERSION=$(awk -F= '/^VERSION_ID=/ {print $2;}' /etc/os-release)

            if [[ "${RELEASE_ID}" == "ubuntu" ]] || [[ "${RELEASE_ID}" == "debian" ]]; then
                INSTALL=apt-get install
            else
                echo "Sorry, $RELEASE_ID Linux is not supported yey."
                exit 1
            fi
            ;;
        solaris*) echo "Sorry, Solaris is not supported yet."; exit 1 ;;
        bsd*)     echo "Sorry, BSD is not supported yet"; exit 1 ;;
        msys*) 
            export OS=Windows
            export RELEASE_ID="MSYS"
            
            if [[ $(echo $CONTITLE | grep MSYS2) ]]; then
                RELEASE_VERSION="MSYS2"
                INSTALL=pacman -S
            else
                echo "Sorry, only MSYS2 is supported"; exit 1
            fi
            ;;
        *)        echo "Sorry, unknown operating system."; exit 1 ;;
     esac
}

function check_install()
{
    if ! has "$1"; then
        echo "$1 is not found. Please install: $SUDO $INSTALL $1"
    else
        echo "found $1"
    fi
}

DECL="$(declare -f download)"
DECL="$DECL; $(declare -f has)"

function run_sudo()
{
    sudo -i bash -c "$DECL; $@"
}

echo -e "Installing Gadget\n"

get_os_type

echo -e "You are running $OS, $RELEASE_ID $RELEASE_VERSION\n"

check_install git
check_install docker

mkdir -p "${INSTALL_DIR}"
download $ALPINE_ROOTFS_URL "${INSTALL_DIR}"/rootfs.tar.gz
cp templates/alpine/blink-leds "${INSTALL_DIR}"/
cp templates/alpine/Dockerfile "${INSTALL_DIR}"/
