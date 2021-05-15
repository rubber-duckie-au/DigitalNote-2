win32 {
	exists($${DIGITALNOTE_LIB_BDB_DIR}/build_unix/libdb_cxx$${BDB_LIB_SUFFIX}.a) {
		message("found BerkeleyDB lib")
	} else {
		message("You need to compile lib BerkeleyDB yourself.")
		message("Also you need to configure the following variables:")
		message("	DIGITALNOTE_LIB_DIR = $${DOLLAR}$${DOLLAR}DIGITALNOTE_PATH/../libs")
		message("	DIGITALNOTE_LIB_BDB_NAME = db-6.2.32.NC")
	}
	
	QMAKE_LIBDIR += $${DIGITALNOTE_LIB_BDB_DIR}/build_unix
	INCLUDEPATH += $${DIGITALNOTE_LIB_BDB_DIR}/build_unix
	DEPENDPATH += $${DIGITALNOTE_LIB_BDB_DIR}/build_unix
}

LIBS += -ldb_cxx$${BDB_LIB_SUFFIX}
