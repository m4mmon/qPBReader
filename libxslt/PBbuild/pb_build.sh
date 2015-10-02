#!/bin/sh

PBTOOLCHAIN=/usr/local/PBSDK11a/FRSCSDK

export CC=arm-none-linux-gnueabi-gcc
export PATH=$PATH:$PBTOOLCHAIN/arm-none-linux-gnueabi/bin:$PBTOOLCHAIN/bin


conf()
{
   ../libxslt-1.1.28/configure \
      --disable-shared \
      --enable-static \
      --without-crypto \
      --host=arm-none-linux-gnueabi \
      --build=i686-pc-linux-gnu \
      --prefix=$PWD/../pb \


}

build()
{
   make
}

install()
{
   make install
}


#conf
#build
install
