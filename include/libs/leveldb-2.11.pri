LIB_PATH = $${DIGITALNOTE_PATH}/src/leveldb-2.11
COMPILE_LEVELDB = 0

exists($${LIB_PATH}/build/libleveldb.a) {
	message("found leveldb lib")
} else {
	!win32 {
		COMPILE_LEVELDB = 1
	} else {
		message("You need to compile leveldb yourself with msys2.")
	}
}

contains(COMPILE_LEVELDB, 1) {
	# we use QMAKE_CXXFLAGS_RELEASE even without RELEASE=1 because we use RELEASE to indicate linking preferences not -O preferences
	extra_leveldb.commands = cd $${LIB_PATH}; mkdir -p build && cd build; cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .
	extra_leveldb.target = $${LIB_PATH}/build/libleveldb.a
	extra_leveldb.depends = FORCE

	PRE_TARGETDEPS += $${LIB_PATH}/build/libleveldb.a
	QMAKE_EXTRA_TARGETS += extra_leveldb

	# Gross ugly hack that depends on qmake internals, unfortunately there is no other way to do it.
	#QMAKE_CLEAN += 
}

QMAKE_LIBDIR += $${LIB_PATH}/build
LIBS += -lleveldb
INCLUDEPATH += $${LIB_PATH}/include
DEPENDPATH += $${LIB_PATH}/include
INCLUDEPATH += $${LIB_PATH}/helpers
DEPENDPATH += $${LIB_PATH}/helpers