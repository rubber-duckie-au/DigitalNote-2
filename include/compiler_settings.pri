QMAKE_CXXFLAGS += -std=c++17
QMAKE_CXXFLAGS = -fdiagnostics-show-option
QMAKE_CXXFLAGS += -fpermissive
#QMAKE_CXXFLAGS += -Wall
#QMAKE_CXXFLAGS += -Wextra
#QMAKE_CXXFLAGS += -H
QMAKE_CXXFLAGS += -Wformat
QMAKE_CXXFLAGS += -Wformat-security
QMAKE_CXXFLAGS += -Wstack-protector
QMAKE_CXXFLAGS += -Wfatal-errors
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-unused-variable
QMAKE_CXXFLAGS += -Wno-ignored-qualifiers
QMAKE_CXXFLAGS += -Wno-unused-local-typedefs

DEPENDPATH += src
INCLUDEPATH += src