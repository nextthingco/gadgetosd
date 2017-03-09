#!/bin/bash

MONGOOSE_URL=https://github.com/cesanta/mongoose/archive/6.7.tar.gz
MONGOOSE_TAR=${MONGOOSE_URL##*/}
MONGOOSE_VER=${MONGOOSE_TAR%.tar.gz}

echo "Downloading mongoose"
wget -c $MONGOOSE_URL
tar xzvf $MONGOOSE_TAR

cp mongoose-$MONGOOSE_VER/mongoose.c .
cp mongoose-$MONGOOSE_VER/mongoose.h .
