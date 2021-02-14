# regenerate src/build.h
!windows|contains(USE_BUILD_INFO, 1)
{
    extra_build.depends = FORCE
    extra_build.commands = cd $$PROJECT_PWD; /bin/sh share/genbuild.sh $$OUT_PWD/build/build.h
    extra_build.target = $$OUT_PWD/build/build.h
    
	PRE_TARGETDEPS += $$OUT_PWD/build/build.h
    QMAKE_EXTRA_TARGETS += extra_build
    DEFINES += HAVE_BUILD_INFO
}
