PROJECT_PWD=$$PWD

TEMPLATE = app
TARGET = DigitalNote-qt
VERSION = 1.0.3.4

OBJECTS_DIR = build/app
MOC_DIR = build/app
UI_DIR = build/app
RCC_DIR = build/app
QMAKE_LINK_OBJECT_SCRIPT = build/app/object_script

QMAKE_LIBDIR += ../libs/boost_1_75_0/stage/lib
QMAKE_LIBDIR += ../libs/openssl-1.0.2u
QMAKE_LIBDIR += ../libs/db-6.2.32.NC/build_unix
QMAKE_LIBDIR += ../libs/libevent-2.1.11-stable/.libs
QMAKE_LIBDIR += ../libs/gmp-6.2.1/.libs

INCLUDEPATH += ../libs/boost_1_75_0
INCLUDEPATH += ../libs/openssl-1.0.2u/include
INCLUDEPATH += ../libs/db-6.2.32.NC/build_unix
INCLUDEPATH += ../libs/libevent-2.1.11-stable/include
INCLUDEPATH += ../libs/gmp-6.2.1

BOOST_LIB_SUFFIX=-mgw7-mt-d-x64-1_75

DEFINES += WIN32

## https://doc.qt.io/qt-5/qmake-variable-reference.html#rc-file
RC_FILE = src/qt/res/bitcoin-qt.rc

include(include/app/qt_settings.pri)
include(include/app/compiler_settings.pri)
include(include/app/forums.pri)
include(include/app/headers.pri)
include(include/app/other_files.pri)
include(include/app/resources.pri)
include(include/app/sources.pri)
include(include/app/translations.pri)

include(include/security.pri)

include(include/options/use_qrcode.pri)
include(include/options/use_0.pri)
include(include/options/use_dbus.pri)
include(include/options/use_upnp.pri)
include(include/options/use_build_info.pri)

include(include/fix/qt.pri)
include(include/fix/boost.pri)
include(include/fix/release.pri)
include(include/fix/msse2.pri)

include(include/extra/leveldb.pri)
include(include/extra/secp256k1.pri)
include(include/extra/openssl.pri)
include(include/extra/gmp.pri)

win32 {
	LIBS += -lshlwapi
	LIBS += -lws2_32
	LIBS += -lmswsock
	LIBS += -lole32
	LIBS += -loleaut32
	LIBS += -luuid
	LIBS += -lgdi32
	LIBS += -pthread
}

system($$QMAKE_LRELEASE -silent $$_PRO_FILE_)