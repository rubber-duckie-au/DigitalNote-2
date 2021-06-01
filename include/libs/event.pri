win32 {
	exists($${DIGITALNOTE_LIB_EVENT_DIR}/.libs/libevent.a) {
		message("found event lib")
	} else {
		message("You need to compile lib event yourself.")
		message("Also you need to configure the following variables:")
		message("	DIGITALNOTE_LIB_GMP_DIR = $${DOLLAR}$${DOLLAR}DIGITALNOTE_PATH/../libs/libevent-2.1.11-stable")
	}
	
	QMAKE_LIBDIR += $${DIGITALNOTE_LIB_EVENT_DIR}/.libs
	INCLUDEPATH += $${DIGITALNOTE_LIB_EVENT_DIR}/include
	DEPENDPATH += $${DIGITALNOTE_LIB_EVENT_DIR}/include
}

macx {
	QMAKE_LIBDIR += $${DIGITALNOTE_LIB_EVENT_DIR}/lib
	INCLUDEPATH += $${DIGITALNOTE_LIB_EVENT_DIR}/include
	DEPENDPATH += $${DIGITALNOTE_LIB_EVENT_DIR}/include
}

LIBS += -levent
