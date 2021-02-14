COMPILE_SECP256K1 = 0

exists($$PROJECT_PWD/src/secp256k1/.libs/libsecp256k1.a) {
	message("found libsecp256k1 lib")
} else {
	COMPILE_secp256k1 = 1
}

contains(COMPILE_SECP256K1, 1) {
	#Build Secp256k1
	# we use QMAKE_CXXFLAGS_RELEASE even without RELEASE=1 because we use RELEASE to indicate linking preferences not -O preferences
	extra_secp256k1.commands = cd $$PROJECT_PWD/src/secp256k1 && chmod 755 ./autogen.sh && ./autogen.sh && ./configure --enable-module-recovery && CC=$$QMAKE_CC CXX=$$QMAKE_CXX $(MAKE) OPT=\"$$QMAKE_CXXFLAGS $$QMAKE_CXXFLAGS_RELEASE\"
	extra_secp256k1.target = $$PROJECT_PWD/src/secp256k1/.libs/libsecp256k1.a
	extra_secp256k1.depends = FORCE

	PRE_TARGETDEPS += $$PROJECT_PWD/src/secp256k1/.libs/libsecp256k1.a
	QMAKE_EXTRA_TARGETS += extra_secp256k1
}