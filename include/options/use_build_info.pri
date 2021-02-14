# regenerate build.h
contains(USE_BUILD_INFO, 1) {
    system("cd $$PROJECT_PWD; rm -f build/build.h || true; /bin/sh share/genbuild.sh build/build.h;")
    
    DEFINES += HAVE_BUILD_INFO
	HEADERS += build/build.h
}
