COMPILE_LEVELDB = 0


exists($$PROJECT_PWD/src/leveldb/libleveldb.a) : exists($$PROJECT_PWD/src/leveldb/libmemenv.a) {
	message("found libleveldb lib")
	message("found libmemenv lib")
} else {
	!win32 {
		COMPILE_LEVELDB = 1
	} else {
		message("You need to compile leveldb yourself with msys2.")
		message("Use the following command to compile:")
		message("	cd src/leveldb")
		message("	TARGET_OS=NATIVE_WINDOWS make libleveldb.a libmemenv.a")
	}
}

contains(COMPILE_LEVELDB, 1) {
	# we use QMAKE_CXXFLAGS_RELEASE even without RELEASE=1 because we use RELEASE to indicate linking preferences not -O preferences
	extra_leveldb.commands = cd $$PROJECT_PWD/src/leveldb && CC=$$QMAKE_CC CXX=$$QMAKE_CXX $(MAKE) OPT=\"$$QMAKE_CXXFLAGS $$QMAKE_CXXFLAGS_RELEASE\" libleveldb.a libmemenv.a
	extra_leveldb.target = $$PROJECT_PWD/src/leveldb/libleveldb.a
	extra_leveldb.depends = FORCE

	PRE_TARGETDEPS += $$PROJECT_PWD/src/leveldb/libleveldb.a
	QMAKE_EXTRA_TARGETS += extra_leveldb

	# Gross ugly hack that depends on qmake internals, unfortunately there is no other way to do it.
	QMAKE_CLEAN += $$PROJECT_PWD/src/leveldb/libleveldb.a; cd $$PROJECT_PWD/src/leveldb ; $(MAKE) clean
}

QMAKE_LIBDIR += src/leveldb
LIBS += -lleveldb
LIBS += -lmemenv