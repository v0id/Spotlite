#-------------------------------------------------
#
# Project created by QtCreator 2011-02-13T15:53:07
#
#-------------------------------------------------

QT       += core gui sql xmlpatterns widgets webkitwidgets network
unix:LIBS += -lcrypto -lz
win32:LIBS += libeay32 -L"C:\openssl\lib\mingw"
win32:QMAKE_CXXFLAGS+=-IC:\OpenSSL\include
win32:DEFINES += MONOLITHIC
win32:QMAKE_LFLAGS_RELEASE += -Wl,--stack,8388608

mac:LIBS  += -L/Developer/SDKs/MacOSX10.5.sdk/usr/lib
mac:CONFIG += x86 x86_64
QMAKE_MACOSX_DEPLOYMENT_TARGET=10.5
mac:LIBS += -lz

TARGET = SpotLite
TEMPLATE = app

TRANSLATIONS += spotlite_en.ts
QTQUICK_COMPILER_SKIPPED_RESOURCES += res/icons.qrc

SOURCES += main.cpp\
        mainwindow.cpp \
    settingsdialog.cpp \
    qtioprocessor/qtiocompressor.cpp \
    spotsmodel.cpp \
    spotlite.cpp \
    spotnntp.cpp \
    spotview.cpp \
    yencmemdecoder.cpp \
    crc.c \
    hashcash/libsha1.c \
    hashcash/lock.c \
    hashcash/random.c \
    hashcash/sstring.c \
    hashcash/utct.c \
    hashcash/libhc.c \
    hashcash/fastmint_library.c \
    hashcash/fastmint_altivec_standard_1.c \
    hashcash/fastmint_altivec_compact_2.c \
    hashcash/fastmint_altivec_standard_2.c \
    hashcash/fastmint_ansi_compact_1.c \
    hashcash/fastmint_ansi_compact_2.c \
    hashcash/fastmint_ansi_standard_1.c \
    hashcash/fastmint_ansi_standard_2.c \
    hashcash/fastmint_ansi_ultracompact_1.c \
    hashcash/fastmint_mmx_compact_1.c \
    hashcash/fastmint_mmx_standard_1.c \
    hashcash/libfastmint.c \
    hashcash/sdb.c \
    hashcash.cpp \
    spotcategories.cpp \
    spotreplywindow.cpp \
    spotsignature.cpp \
    uisettingsdialog.cpp \
    filterdialog.cpp \
    crashdumper.cpp \
    customfilter.cpp \
    addressdialog.cpp \
    userdialog.cpp \
    parsedate.c \
    json/serializer.cpp \
    SpotLite-skininstaller/skininstaller.cpp \
    SpotLite-skininstaller/zip.cpp \
    spotimagelist.cpp \
    cachewarmer.cpp \
    spotreportwindow.cpp \
    dummyitemdelegate.cpp \
    spotbrowser.cpp

HEADERS  += mainwindow.h \
    settingsdialog.h \
    qtioprocessor/zconf.h \
    qtioprocessor/qtiocompressor.h \
    qtioprocessor/zlib.h \
    spotsmodel.h \
    spotlite.h \
    spotnntp.h \
    spotview.h \
    yencmemdecoder.h \
    crc.h \
    hashcash/hashcash.h \
    hashcash.h \
    spotcategories.h \
    spotreplywindow.h \
    spotsignature.h \
    uisettingsdialog.h \
    filterdialog.h \
    crashdumper.h \
    customfilter.h \
    addressdialog.h \
    userdialog.h \
    parsedate.h \
#    spotmobile.h \
#    spotmobilehandler.h \
    json/serializer.h \
#    spotmobileview.h \
#    spotmobilesettings.h \
    SpotLite-skininstaller/zip.h \
    SpotLite-skininstaller/skininstaller.h \
    spotimagelist.h \
    cachewarmer.h \
    spotreportwindow.h \
    dummyitemdelegate.h \
    spotbrowser.h

FORMS    += mainwindow.ui \
    settingsdialog.ui \
    spotreplywindow.ui \
    uisettingsdialog.ui \
    filterdialog.ui \
    addressdialog.ui \
    userdialog.ui
#    \ spotmobilesettings.ui

RESOURCES += \
    res/icons.qrc

win32:RC_FILE += \
    res/windows.rc

mac:RC_FILE += \
    res/icon.icns
QMAKE_INFO_PLIST = spotlite.plist

QMAKE_CXXFLAGS+=-g

OTHER_FILES += \
    res/spot.tpl \
    res/comment.tpl \
    Changelog.txt \
    res/windows.rc \
    res/SpotLite.manifest \
    res/mobilespot.tpltxt \
    res/mobilespot.tpl \
    spotlite.plist \
    res/imagelist.tpl \
    res/imagelistitem.tpl
