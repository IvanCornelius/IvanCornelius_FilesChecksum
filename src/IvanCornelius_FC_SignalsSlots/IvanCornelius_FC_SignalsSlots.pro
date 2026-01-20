QT += core
QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

TARGET = checksum_signals

SOURCES += \
    IvanCornelius_FilesChecksum.cpp \
    IvanCornelius_OneFileChecksum.cpp \
    main.cpp

HEADERS += \
    IvanCornelius_FilesChecksum.hpp \
    IvanCornelius_OneFileChecksum.hpp
