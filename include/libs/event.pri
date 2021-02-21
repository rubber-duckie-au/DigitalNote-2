win32 {
	LIB_PATH = $${DIGITALNOTE_LIB_DIR}/$${DIGITALNOTE_LIB_EVENT_NAME}
	
	exists($${LIB_PATH}/.libs/libevent.a) {
		message("found event lib")
	} else {
		message("You need to compile lib event yourself.")
		message("Also you need to configure the following variables:")
		message("	DIGITALNOTE_LIB_DIR = $${DOLLAR}$${DOLLAR}DIGITALNOTE_PATH/../libs")
		message("	DIGITALNOTE_LIB_EVENT_NAME = libevent-2.1.11-stable")
	}
	
	QMAKE_LIBDIR += $${LIB_PATH}/.libs
	INCLUDEPATH += $${LIB_PATH}/include
	DEPENDPATH += $${LIB_PATH}/include
}

LIBS += -levent
