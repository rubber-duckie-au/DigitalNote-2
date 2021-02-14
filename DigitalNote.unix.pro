PROJECT_PWD=$$PWD

TEMPLATE = app
TARGET = DigitalNote-qt
VERSION = 1.0.3.4

OBJECTS_DIR = build
MOC_DIR = build 
UI_DIR = build

include(include/qt_settings.pri)
include(include/compiler_settings.pri)

include(include/sources.pri)
include(include/headers.pri)
include(include/forums.pri)
include(include/resources.pri)
include(include/translations.pri)

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
##include(include/extra/TSQM.pri)

system($$QMAKE_LRELEASE -silent $$_PRO_FILE_)