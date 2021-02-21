LIB_PATH = $${DIGITALNOTE_PATH}/src/leveldb
COMPILE_LEVELDB = 0

exists($${LIB_PATH}/libleveldb.a) : exists($${LIB_PATH}/libmemenv.a) {
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
	extra_leveldb.commands = cd $${LIB_PATH} && CC=$$QMAKE_CC CXX=$$QMAKE_CXX $(MAKE) OPT=\"$$QMAKE_CXXFLAGS $$QMAKE_CXXFLAGS_RELEASE\" libleveldb.a libmemenv.a
	extra_leveldb.target = $${LIB_PATH}/libleveldb.a
	extra_leveldb.depends = FORCE

	PRE_TARGETDEPS += $${LIB_PATH}/libleveldb.a
	QMAKE_EXTRA_TARGETS += extra_leveldb

	# Gross ugly hack that depends on qmake internals, unfortunately there is no other way to do it.
	QMAKE_CLEAN += $${LIB_PATH}/libleveldb.a; cd $${LIB_PATH}; $(MAKE) clean
}

QMAKE_LIBDIR += $${LIB_PATH}
LIBS += -lleveldb
LIBS += -lmemenv
INCLUDEPATH += $${LIB_PATH}/include
DEPENDPATH += $${LIB_PATH}/include
INCLUDEPATH += $${LIB_PATH}/helpers
DEPENDPATH += $${LIB_PATH}/helpers