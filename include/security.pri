# for extra security against potential buffer overflows: enable GCCs Stack Smashing Protection
QMAKE_CXXFLAGS *= -fstack-protector-all --param ssp-buffer-size=1
QMAKE_LFLAGS *= -fstack-protector-all --param ssp-buffer-size=1

# for extra security (see: https://wiki.debian.org/Hardening): this flag is GCC compiler-specific
QMAKE_CXXFLAGS += -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2

# main.o too many sections
QMAKE_CXXFLAGS += -Wl,-allow-multiple-definition
