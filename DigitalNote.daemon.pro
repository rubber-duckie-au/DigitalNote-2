include(include/definitions.pri)

DIGITALNOTE_APP_NAME = daemon
DIGITALNOTE_PATH = $$PWD

include(DigitalNote_config.pri)

include(include/base.pri)

TARGET = DigitalNote

DEFINES += ENABLE_WALLET

include(include/compiler_settings.pri)
include(include/security.pri)
include(include/qt.pri)
include(include/release.pri)
include(include/msse2.pri)

include(include/daemon/qt_settings.pri)
include(include/daemon/headers.pri)
include(include/daemon/sources.pri)

## Compile Options
include(include/options/use_0.pri)
include(include/options/use_build_info.pri)

## Libraries
include(include/libs.pri)

system($$QMAKE_LRELEASE -silent $$_PRO_FILE_)
