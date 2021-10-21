TARGET = QtMagicBlueLed

CONFIG += c++11

QT = core bluetooth

QT += core-private

PUBLIC_HEADERS += qmagicblueledglobal.h qmagicblueledclient.h qmagicblueled.h

PRIVATE_HEADERS += 

SOURCES += qmagicblueledclient.cpp qmagicblueled.cpp

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

load(qt_module)
