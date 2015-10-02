#!/bin/sh
export LD_LIBRARY_PATH=/mnt/ext1/system/share/qPBReader/lib
export FONTCONFIG_FILE=/mnt/ext1/system/config/qPBReaderFonts.conf
exec /mnt/ext1/system/share/qPBReader/bin/qPBReader "$@" 1> /mnt/ext1/system/share/qPBReader/qPBReader.log 2>&1
