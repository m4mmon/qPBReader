TEMPLATE = subdirs
SUBDIRS = \
          miniunz \
          qPBReader \

miniunz.subdir = miniunz

qPBReader.subdir = qPBReader
qPBReader.depends = miniunz

QMAKE_DISTCLEAN += -r bin
