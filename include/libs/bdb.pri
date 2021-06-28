win32 {
	exists($${DIGITALNOTE_LIB_BDB_DIR}/build_unix/libdb_cxx$${DIGITALNOTE_LIB_BDB_SUFFIX}.a) {
		message("found BerkeleyDB lib")
	} else {
		message("You need to compile lib BerkeleyDB yourself.")
		message("Also you need to configure the following variables:")
		message("	DIGITALNOTE_LIB_BDB_DIR = $${DOLLAR}$${DOLLAR}DIGITALNOTE_PATH/../libs/db-6.2.32.NC")
	}
	
	QMAKE_LIBDIR += $${DIGITALNOTE_LIB_BDB_DIR}/build_unix
	INCLUDEPATH += $${DIGITALNOTE_LIB_BDB_DIR}/build_unix
	DEPENDPATH += $${DIGITALNOTE_LIB_BDB_DIR}/build_unix
}

macx {
	QMAKE_LIBDIR += $${DIGITALNOTE_LIB_BDB_DIR}/lib
	INCLUDEPATH += $${DIGITALNOTE_LIB_BDB_DIR}/include
	DEPENDPATH += $${DIGITALNOTE_LIB_BDB_DIR}/include
}

LIBS += -ldb_cxx$${DIGITALNOTE_LIB_BDB_SUFFIX}
