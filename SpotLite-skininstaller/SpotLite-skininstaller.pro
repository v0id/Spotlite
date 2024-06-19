#-------------------------------------------------
#
# Project created by QtCreator 2011-03-26T20:39:25
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = SpotLite-skininstaller
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    zip.cpp \
    skininstaller.cpp \
    qtioprocessor/qtiocompressor.cpp

HEADERS += \
    zip.h \
    skininstaller.h \
    qtioprocessor/qtiocompressor.h

OTHER_FILES += \
    windows-uac.rc \
    SpotLite-skininstaller.manifest

win32:RC_FILE += \
    windows-uac.rc
