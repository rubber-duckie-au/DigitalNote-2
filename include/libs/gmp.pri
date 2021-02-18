win32 {
	LIB_PATH = $${DIGITALNOTE_LIB_DIR}/$${DIGITALNOTE_LIB_GMP_NAME}
	
	exists($${LIB_PATH}/.libs/libgmp.a) {
		message("found gmp lib")
	} else {
		message("You need to compile leveldb yourself with msys2.")
		message("Also you need to configure the following variables:")
		message("	DIGITALNOTE_LIB_DIR = $${DOLLAR}$${DOLLAR}DIGITALNOTE_PATH/../libs")
		message("	DIGITALNOTE_LIB_GMP_NAME = gmp-6.2.1")
	}
	
	QMAKE_LIBDIR += $${LIB_PATH}/.libs
	INCLUDEPATH += $${LIB_PATH}
}

LIBS += -lgmp
