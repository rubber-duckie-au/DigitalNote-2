# regenerate build.h
contains(USE_BUILD_INFO, 1) {
	win32 {
		system("del $${DIGITALNOTE_PATH}/build/build.h > nul 2> nul")
		system("$${DIGITALNOTE_PATH}/share/genbuild.bat $${DIGITALNOTE_PATH}/build/build.h")
	} else {
		system("cd $$DIGITALNOTE_PATH; rm -f build/build.h || true; /bin/sh share/genbuild.sh build/build.h;")
	}
	
	DEFINES += HAVE_BUILD_INFO
	HEADERS += build/build.h
}
