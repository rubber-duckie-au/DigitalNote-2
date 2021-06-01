win32 {
	exists($${DIGITALNOTE_LIB_GMP_DIR}/.libs/libgmp.a) {
		message("found gmp lib")
	} else {
		message("You need to compile leveldb yourself with msys2.")
		message("Also you need to configure the following variables:")
		message("	DIGITALNOTE_LIB_GMP_DIR = $${DOLLAR}$${DOLLAR}DIGITALNOTE_PATH/../libs/gmp-6.2.1")
	}
	
	QMAKE_LIBDIR += $${DIGITALNOTE_LIB_GMP_DIR}/.libs
	INCLUDEPATH += $${DIGITALNOTE_LIB_GMP_DIR}
	DEPENDPATH += $${DIGITALNOTE_LIB_GMP_DIR}
}

macx {
	QMAKE_LIBDIR += $${DIGITALNOTE_LIB_GMP_DIR}/lib
	INCLUDEPATH += $${DIGITALNOTE_LIB_GMP_DIR}/include
	DEPENDPATH += $${DIGITALNOTE_LIB_GMP_DIR}/include
}

LIBS += -lgmp
