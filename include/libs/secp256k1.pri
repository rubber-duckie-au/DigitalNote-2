COMPILE_SECP256K1 = 0

exists($${DIGITALNOTE_LIB_SECP256K1_DIR}/.libs/libsecp256k1.a) {
	message("found secp256k1 lib")
} else {
	!win32 {
		COMPILE_SECP256K1 = 1
	} else {
		message("You need to compile secp256k1 yourself with msys2.")
	}
}


contains(COMPILE_SECP256K1, 1) {
	#Build Secp256k1
	# we use QMAKE_CXXFLAGS_RELEASE even without RELEASE=1 because we use RELEASE to indicate linking preferences not -O preferences
	extra_secp256k1.commands = cd $${DIGITALNOTE_LIB_SECP256K1_DIR} && chmod 755 ./autogen.sh && ./autogen.sh && ./configure --enable-module-recovery && $(MAKE) clean && CC=$$QMAKE_CC CXX=$$QMAKE_CXX $(MAKE) OPT=\"$$QMAKE_CXXFLAGS $$QMAKE_CXXFLAGS_RELEASE\"
	extra_secp256k1.target = $${DIGITALNOTE_LIB_SECP256K1_DIR}/.libs/libsecp256k1.a
	extra_secp256k1.depends = FORCE

	PRE_TARGETDEPS += $${DIGITALNOTE_LIB_SECP256K1_DIR}/.libs/libsecp256k1.a
	QMAKE_EXTRA_TARGETS += extra_secp256k1
}

##
## We dont use -l<name> because at linux the gcc compiler takes .so.1 file first instead we need to .a lib file.
##
LIBS += $${DIGITALNOTE_LIB_SECP256K1_DIR}/.libs/libsecp256k1.a
INCLUDEPATH += $${DIGITALNOTE_LIB_SECP256K1_DIR}/include
DEPENDPATH += $${DIGITALNOTE_LIB_SECP256K1_DIR}/include