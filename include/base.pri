TEMPLATE = app

VERSION = "$${DIGITALNOTE_VERSION_MAJOR}.$${DIGITALNOTE_VERSION_MINOR}.$${DIGITALNOTE_VERSION_REVISION}.$${DIGITALNOTE_VERSION_BUILD}"

## Set build directories
OBJECTS_DIR = build/$${DIGITALNOTE_APP_NAME}
MOC_DIR = build/$${DIGITALNOTE_APP_NAME}
UI_DIR = build/$${DIGITALNOTE_APP_NAME}
RCC_DIR = build/$${DIGITALNOTE_APP_NAME}
QMAKE_LINK_OBJECT_SCRIPT = build/$${DIGITALNOTE_APP_NAME}/object_script

## Define OS
win32 {
	DEFINES += WIN32
	
	## https://doc.qt.io/qt-5/qmake-variable-reference.html#rc-file
	RC_FILE = src/qt/res/bitcoin-qt.rc
}

unix {
	DEFINES += LINUX
}

macx {
	DEFINES += MAC_OSX
	DEFINES += MSG_NOSIGNAL=0
	
	ICON = src/qt/res/icons/digitalnote.icns
}