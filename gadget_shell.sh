#!/bin/bash

clear

export SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export PATH="$SCRIPTDIR:$PATH"
export PS1="GADGET: \w$ "

if [[ "$(which docker)" == "" ]];
then
    echo -e "\
\n\
\n\
**************************************************** \n\
 Gadget requires Docker installed. Please install!   \n\
**************************************************** \n\

Follow the instructions on this web page:
https://store.docker.com/editions/community/docker-ce-desktop-mac
\n\
\n\
Press ENTER to continue
"
read
open https://store.docker.com/editions/community/docker-ce-desktop-mac
else
    echo -e "\
\n\
\n\
******************************************************************* \n\
 Welcome to Gadget!                                                 \n\
******************************************************************* \n\
\n\
Please flash your CHIP PRO with the latest Gadget OS image.\n\
Then follow these easy steps to deploy your first application:\n\
\n\
gadget init hello_world\n\
cd hello_world\n\
gadget build\n\
gadget deploy\n\
"
    bash -i
fi
