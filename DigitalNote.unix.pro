PROJECT_PWD=$$PWD

TEMPLATE = app
TARGET = DigitalNote-qt
VERSION = 1.0.3.4

OBJECTS_DIR = build/App
MOC_DIR = build/App
UI_DIR = build/App

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

system($$QMAKE_LRELEASE -silent $$_PRO_FILE_)