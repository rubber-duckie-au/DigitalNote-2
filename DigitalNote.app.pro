include(include/definitions.pri)

DIGITALNOTE_APP_NAME = app
DIGITALNOTE_PATH = $$PWD

include(DigitalNote_config.pri)

include(include/base.pri)

TARGET = DigitalNote-qt

DEFINES += ENABLE_WALLET

include(include/build_path.pri)
include(include/compiler_settings.pri)
include(include/security.pri)
include(include/qt.pri)
include(include/release.pri)
include(include/msse2.pri)

include(include/app/qt_settings.pri)
include(include/app/forums.pri)
include(include/app/headers.pri)
include(include/app/other_files.pri)
include(include/app/resources.pri)
include(include/app/sources.pri)
include(include/app/translations.pri)

## Compile Options
include(include/options/use_update_alternatives.pri)
include(include/options/use_0.pri)
include(include/options/use_build_info.pri)
include(include/options/use_dbus.pri)
include(include/options/use_pch.pri)
include(include/options/use_unity_build.pri)

## Libraries
include(include/libs.pri)

system($$QMAKE_LRELEASE -silent $$_PRO_FILE_)
