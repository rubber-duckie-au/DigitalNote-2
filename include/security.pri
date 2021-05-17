!win32 {
	# for extra security against potential buffer overflows: enable GCCs Stack Smashing Protection
	QMAKE_CXXFLAGS *= -fstack-protector-all --param ssp-buffer-size=1
	QMAKE_LFLAGS *= -fstack-protector-all --param ssp-buffer-size=1
} else {
	# for extra security on Windows: enable ASLR and DEP via GCC linker flags
	QMAKE_LFLAGS += -Wl,--dynamicbase
	QMAKE_LFLAGS += -Wl,--nxcompat
	
	## Only for 32 bit mingw compiler
	##QMAKE_LFLAGS += -Wl,--large-address-aware
	
	# main.o too many sections
	QMAKE_CXXFLAGS += -Wl,-allow-multiple-definition
	
	# on Windows: enable GCC large address aware linker flag
	QMAKE_LFLAGS += -static
	
	# i686-w64-mingw32
	QMAKE_LFLAGS += -static-libgcc
	QMAKE_LFLAGS += -static-libstdc++
}

# for extra security (see: https://wiki.debian.org/Hardening): this flag is GCC compiler-specific
QMAKE_CXXFLAGS += -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2


