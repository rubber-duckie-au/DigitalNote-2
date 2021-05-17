win32 {
	FAIL = 0
	
	exists($${DIGITALNOTE_LIB_OPENSSL_DIR}/libssl.a) {
		message("found ssl lib")
	} else {
		FAIL = 1
	}
	
	exists($${DIGITALNOTE_LIB_OPENSSL_DIR}/libcrypto.a) {
		message("found crypto lib")
	} else {
		FAIL = 1
	}
	
	contains(FAIL, 1) {
		message("You need to compile openssl yourself with msys2.")
		message("Also you need to configure the following variables:")
		message("	DIGITALNOTE_LIB_QRENCODE_DIR = $${DOLLAR}$${DOLLAR}DIGITALNOTE_PATH/../libs/openssl-1.0.2u")
	}
	
	QMAKE_LIBDIR += $${DIGITALNOTE_LIB_OPENSSL_DIR}
	INCLUDEPATH += $${DIGITALNOTE_LIB_OPENSSL_DIR}/include
	DEPENDPATH += $${DIGITALNOTE_LIB_OPENSSL_DIR}/include
}

macx {
	QMAKE_LIBDIR += $${DIGITALNOTE_LIB_OPENSSL_DIR}/lib
	INCLUDEPATH += $${DIGITALNOTE_LIB_OPENSSL_DIR}/include
	DEPENDPATH += $${DIGITALNOTE_LIB_OPENSSL_DIR}/include
}

LIBS += -lssl
LIBS += -lcrypto
