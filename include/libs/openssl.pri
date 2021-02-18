win32 {
	LIB_PATH = $${DIGITALNOTE_LIB_DIR}/$${DIGITALNOTE_LIB_OPENSSL_NAME}
	FAIL = 0
	
	exists($${LIB_PATH}/libssl.a) {
		message("found ssl lib")
	} else {
		FAIL = 1
	}
	
	exists($${LIB_PATH}/libcrypto.a) {
		message("found crypto lib")
	} else {
		FAIL = 1
	}
	
	contains(FAIL, 1) {
		message("You need to compile openssl yourself with msys2.")
		message("Also you need to configure the following variables:")
		message("	DIGITALNOTE_LIB_DIR = $${DOLLAR}$${DOLLAR}PROJECT_PWD/../libs")
		message("	DIGITALNOTE_LIB_OPENSSL_NAME = openssl-1.0.2u")
	}
	
	QMAKE_LIBDIR += $${LIB_PATH}
	INCLUDEPATH += $${LIB_PATH}/include
}

LIBS += -lssl
LIBS += -lcrypto
