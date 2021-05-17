win32 {
	FAIL = 0
	
	exists($${DIGITALNOTE_LIB_BOOST_DIR}/stage/lib/libboost_system$${DIGITALNOTE_LIB_BOOST_SUFFIX}.a) {
		message("found boost system lib")
	} else {
		FAIL = 1
	}
	
	exists($${DIGITALNOTE_LIB_BOOST_DIR}/stage/lib/libboost_filesystem$${DIGITALNOTE_LIB_BOOST_SUFFIX}.a) {
		message("found boost filesystem lib")
	} else {
		FAIL = 1
	}
	
	exists($${DIGITALNOTE_LIB_BOOST_DIR}/stage/lib/libboost_program_options$${DIGITALNOTE_LIB_BOOST_SUFFIX}.a) {
		message("found boost program options lib")
	} else {
		FAIL = 1
	}
	
	exists($${DIGITALNOTE_LIB_BOOST_DIR}/stage/lib/libboost_thread$${DIGITALNOTE_LIB_BOOST_SUFFIX}.a) {
		message("found boost thread lib")
	} else {
		FAIL = 1
	}
	
	exists($${DIGITALNOTE_LIB_BOOST_DIR}/stage/lib/libboost_chrono$${DIGITALNOTE_LIB_BOOST_SUFFIX}.a) {
		message("found boost chrono lib")
	} else {
		FAIL = 1
	}
	
	contains(FAIL, 1) {
		message("You need to compile boost yourself.")
		message("Also you need to configure the following variables:")
		message("	DIGITALNOTE_LIB_DIR = $${DOLLAR}$${DOLLAR}DIGITALNOTE_PATH/../libs")
		message("	DIGITALNOTE_LIB_BOOST_NAME = boost_1_75_0")
		message("	DIGITALNOTE_LIB_BOOST_SUFFIX = -mgw7-mt-d-x64-1_75")
	}
	
	QMAKE_LIBDIR += $${DIGITALNOTE_LIB_BOOST_DIR}/stage/lib
	INCLUDEPATH += $${DIGITALNOTE_LIB_BOOST_DIR}
	DEPENDPATH += $${DIGITALNOTE_LIB_BOOST_DIR}
}

macx {
	QMAKE_LIBDIR += $${DIGITALNOTE_LIB_BOOST_DIR}/lib
	INCLUDEPATH += $${DIGITALNOTE_LIB_BOOST_DIR}/include
	DEPENDPATH += $${DIGITALNOTE_LIB_BOOST_DIR}/include
}

LIBS += -lboost_system$${DIGITALNOTE_LIB_BOOST_SUFFIX}
LIBS += -lboost_filesystem$${DIGITALNOTE_LIB_BOOST_SUFFIX}
LIBS += -lboost_program_options$${DIGITALNOTE_LIB_BOOST_SUFFIX}
LIBS += -lboost_thread$${DIGITALNOTE_LIB_BOOST_SUFFIX}
LIBS += -lboost_chrono$${DIGITALNOTE_LIB_BOOST_SUFFIX}

DEFINES += BOOST_THREAD_USE_LIB
DEFINES += BOOST_SPIRIT_THREADSAFE

# workaround for boost 1.58
DEFINES += BOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT
DEFINES += BOOST_BIND_GLOBAL_PLACEHOLDERS
DEFINES += BOOST_ALLOW_DEPRECATED_HEADERS

# At least qmake's win32-g++-cross profile is missing the -lmingwthrd
# thread-safety flag. GCC has -mthreads to enable this, but it doesn't
# work with static linking. -lmingwthrd must come BEFORE -lmingw, so
# it is prepended to QMAKE_LIBS_QT_ENTRY.
# It can be turned off with MINGW_THREAD_BUGFIX=0, just in case it causes
# any problems on some untested qmake profile now or in the future.

windows:!contains(MINGW_THREAD_BUGFIX, 0) {
    DEFINES += _MT BOOST_THREAD_PROVIDES_GENERIC_SHARED_MUTEX_ON_WIN
    QMAKE_LIBS_QT_ENTRY = -lmingwthrd $$QMAKE_LIBS_QT_ENTRY
}
