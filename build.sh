#!/bin/bash

echo -e "\
## NOTE: while mongoose / libmongoose can be installed on some systems\n\
##       via a package manager, this is not really useful for our project.\n\
##       We need the exact version 6.7 and need it compiled with the flag\n\
##          -DMG_ENABLE_HTTP_STREAMING_MULTIPART\n\n
"

#### so outside buildroot, we do:
CC=${CC:-gcc}
AR=${AR:-ar}
    
MONGOOSE_URL=https://github.com/cesanta/mongoose/archive/6.7.tar.gz
MONGOOSE_TAR=${MONGOOSE_URL##*/}
MONGOOSE_VER=${MONGOOSE_TAR%.tar.gz}

MONGOOSE_FLAGS=-DMG_ENABLE_HTTP_STREAMING_MULTIPART

echo "Downloading mongoose"
wget -c $MONGOOSE_URL
tar xzvf $MONGOOSE_TAR

cp mongoose-$MONGOOSE_VER/mongoose.c .
cp mongoose-$MONGOOSE_VER/mongoose.h .

${CC} -c mongoose.c ${MONGOOSE_CFLAGS} -o mongoose.o
${AR} rcs libmongoose.a mongoose.o

make
