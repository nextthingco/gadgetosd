#!/bin/bash

#### does not work because mongoose needs to be compiled with =-DMG_ENABLE_HTTP_STREAMING_MULTIPART :
# on mac, do `brew install mongoose`
# make sure to havev version 6.7: `brew upgrade mongoose; brew switch mongoose 6.7`

#### so outside buildroot, we do:
MONGOOSE_URL=https://github.com/cesanta/mongoose/archive/6.7.tar.gz
MONGOOSE_TAR=${MONGOOSE_URL##*/}
MONGOOSE_VER=${MONGOOSE_TAR%.tar.gz}

echo "Downloading mongoose"
wget -c $MONGOOSE_URL
tar xzvf $MONGOOSE_TAR

cp mongoose-$MONGOOSE_VER/mongoose.c .
cp mongoose-$MONGOOSE_VER/mongoose.h .

make
