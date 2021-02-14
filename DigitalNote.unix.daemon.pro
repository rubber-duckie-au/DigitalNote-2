PROJECT_PWD=$$PWD

TEMPLATE = app
TARGET = DigitalNote
VERSION = 1.0.3.4

OBJECTS_DIR = build/daemon

include(include/daemon/qt_settings.pri)
include(include/daemon/compiler_settings.pri)
include(include/daemon/headers.pri)
include(include/daemon/sources.pri)

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