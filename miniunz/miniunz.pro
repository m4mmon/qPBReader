#
# miniunz static lib
#

QT               =

TEMPLATE         = lib

CONFIG          += staticlib

HEADERS          = \
                   include/ioapi.h \
                   include/unzip.h \


SOURCES          = \
                   src/ioapi.c \
                   src/unzip.c \


INCLUDEPATH     += include

#LIBS            += -lz

DEFINES         += NOUNCRYPT

OBJECTS_DIR      = obj

QMAKE_DISTCLEAN += -r obj lib

#CONFIG          += debug


TARGET           = lib/miniunz
