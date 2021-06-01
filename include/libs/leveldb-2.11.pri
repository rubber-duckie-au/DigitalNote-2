!win32 {
	COMPILE_LEVELDB = 0
	
	exists($${DIGITALNOTE_LIB_LEVELDB_NEW_DIR}/build/libleveldb.a) {
		message("found leveldb lib")
	} else {
		COMPILE_LEVELDB = 1
	}
	
	contains(COMPILE_LEVELDB, 1) {
		# we use QMAKE_CXXFLAGS_RELEASE even without RELEASE=1 because we use RELEASE to indicate linking preferences not -O preferences
		extra_leveldb.commands = cd $${DIGITALNOTE_LIB_LEVELDB_NEW_DIR}; mkdir -p build && cd build; cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .
		extra_leveldb.target = $${DIGITALNOTE_LIB_LEVELDB_NEW_DIR}/build/libleveldb.a
		extra_leveldb.depends = FORCE

		PRE_TARGETDEPS += $${DIGITALNOTE_LIB_LEVELDB_NEW_DIR}/build/libleveldb.a
		QMAKE_EXTRA_TARGETS += extra_leveldb

		# Gross ugly hack that depends on qmake internals, unfortunately there is no other way to do it.
		#QMAKE_CLEAN += 
	}
}

win32 {
	exists($${DIGITALNOTE_LIB_LEVELDB_NEW_DIR}/build/libleveldb.a) {
		message("found leveldb lib")
	} else {
		message("You need to compile leveldb yourself with msys2.")
	}
}

QMAKE_LIBDIR += $${DIGITALNOTE_LIB_LEVELDB_NEW_DIR}/build

INCLUDEPATH += $${DIGITALNOTE_LIB_LEVELDB_NEW_DIR}/include
DEPENDPATH += $${DIGITALNOTE_LIB_LEVELDB_NEW_DIR}/include
INCLUDEPATH += $${DIGITALNOTE_LIB_LEVELDB_NEW_DIR}/helpers
DEPENDPATH += $${DIGITALNOTE_LIB_LEVELDB_NEW_DIR}/helpers

LIBS += -lleveldb