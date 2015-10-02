#!/bin/sh


#../libxslt-1.1.28/configure --help 1> cfg.txt 2>&1

conf()
{
   ../libxslt-1.1.28/configure \
      --disable-shared \
      --enable-static \
      --prefix=$PWD/../pc


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
build
install
