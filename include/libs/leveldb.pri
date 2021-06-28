COMPILE_LEVELDB = 0

exists($${DIGITALNOTE_LIB_LEVELDB_DIR}/libleveldb.a) : exists($${DIGITALNOTE_LIB_LEVELDB_DIR}/libmemenv.a) {
	message("found leveldb lib")
	message("found memenv lib")
} else {
	!win32 {
		COMPILE_LEVELDB = 1
	} else {
		message("You need to compile leveldb yourself with msys2.")
	}
}

contains(COMPILE_LEVELDB, 1) {
	# we use QMAKE_CXXFLAGS_RELEASE even without RELEASE=1 because we use RELEASE to indicate linking preferences not -O preferences
	extra_leveldb.commands = cd $${DIGITALNOTE_LIB_LEVELDB_DIR} && CC=$$QMAKE_CC CXX=$$QMAKE_CXX $(MAKE) OPT=\"$$QMAKE_CXXFLAGS $$QMAKE_CXXFLAGS_RELEASE\" libleveldb.a libmemenv.a
	extra_leveldb.target = $${DIGITALNOTE_LIB_LEVELDB_DIR}/libleveldb.a
	extra_leveldb.depends = FORCE

	PRE_TARGETDEPS += $${DIGITALNOTE_LIB_LEVELDB_DIR}/libleveldb.a
	QMAKE_EXTRA_TARGETS += extra_leveldb

	# Gross ugly hack that depends on qmake internals, unfortunately there is no other way to do it.
	QMAKE_CLEAN += $${DIGITALNOTE_LIB_LEVELDB_DIR}/libleveldb.a; cd $${DIGITALNOTE_LIB_LEVELDB_DIR}; $(MAKE) clean
}

QMAKE_LIBDIR += $${DIGITALNOTE_LIB_LEVELDB_DIR}

INCLUDEPATH += $${DIGITALNOTE_LIB_LEVELDB_DIR}/include
DEPENDPATH += $${DIGITALNOTE_LIB_LEVELDB_DIR}/include
INCLUDEPATH += $${DIGITALNOTE_LIB_LEVELDB_DIR}/helpers
DEPENDPATH += $${DIGITALNOTE_LIB_LEVELDB_DIR}/helpers

LIBS += -lleveldb
LIBS += -lmemenv