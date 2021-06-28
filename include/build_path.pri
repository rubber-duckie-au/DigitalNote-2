OBJECTS_DIR = build/app_and_daemon
MOC_DIR = build/app_and_daemon
UI_DIR = build/app_and_daemon
RCC_DIR = build/app_and_daemon
QMAKE_LINK_OBJECT_SCRIPT = build/app_and_daemon/object_script

contains(USE_PCH, 1) {
	## Set build directories
	OBJECTS_DIR = build/$${DIGITALNOTE_APP_NAME}
	MOC_DIR = build/$${DIGITALNOTE_APP_NAME}
	UI_DIR = build/$${DIGITALNOTE_APP_NAME}
	RCC_DIR = build/$${DIGITALNOTE_APP_NAME}
	QMAKE_LINK_OBJECT_SCRIPT = build/$${DIGITALNOTE_APP_NAME}/object_script
}