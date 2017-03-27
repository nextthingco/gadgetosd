#!/bin/bash

brew_install() 
{
    PACKAGE=$1
    if [[ -z "$PACKAGE" ]]; then
        return 1;
    fi

    brew ls --versions $PACKAGE > /dev/null || brew install $PACKAGE || exit 1
}

brew_install cmake
brew_install librsvg
brew_install ninja
brew_install gettext
brew link --force gettext
