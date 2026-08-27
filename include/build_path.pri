OBJECTS_DIR = build/app_and_daemon
MOC_DIR = build/app_and_daemon
UI_DIR = build/app_and_daemon
RCC_DIR = build/app_and_daemon

contains(USE_PCH, 1) {
	## Set build directories
	OBJECTS_DIR = build/$${DIGITALNOTE_APP_NAME}
	MOC_DIR = build/$${DIGITALNOTE_APP_NAME}
	UI_DIR = build/$${DIGITALNOTE_APP_NAME}
	RCC_DIR = build/$${DIGITALNOTE_APP_NAME}
}

## v2.0.0.9 Qt6: QMAKE_LINK_OBJECT_SCRIPT is now a BARE FILENAME.
##
## When the link command line exceeds QMAKE_LINK_OBJECT_MAX objects, qmake
## writes the object list to a response file named
##     <dir>/$${QMAKE_LINK_OBJECT_SCRIPT}.$${TARGET}
##
## Qt5 resolved <dir> as the build root, so carrying the path in the value
## worked.  Qt6 resolves it relative to OBJECTS_DIR, so the same value produced
## a DOUBLED path and the link died with:
##
##   Error: Cannot open response file
##   '.../DigitalNote-2\build/app_and_daemon/build/app_and_daemon/object_script.DigitalNoted'
##   for writing.
##
## The directory does not exist, hence "cannot open ... for writing" rather
## than a path error, which makes it look like a permissions problem.
##
## The bare name is correct on BOTH: Qt5 puts it at the build root, Qt6 inside
## OBJECTS_DIR.  Either is fine -- nothing else refers to this file by path,
## qmake emits the reference into the generated Makefile itself.  So no version
## guard is needed, and adding one would only invite the doubled value back.
##
## It is set AFTER the USE_PCH block on purpose: OBJECTS_DIR may change there,
## and this value must not encode whatever OBJECTS_DIR happened to be.
QMAKE_LINK_OBJECT_SCRIPT = object_script