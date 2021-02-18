QMAKE_CXXFLAGS_WARN_ON += -std=c++17
QMAKE_CXXFLAGS_WARN_ON = -fdiagnostics-show-option
QMAKE_CXXFLAGS_WARN_ON += -fpermissive
#QMAKE_CXXFLAGS_WARN_ON += -Wall
#QMAKE_CXXFLAGS_WARN_ON += -Wextra
#QMAKE_CXXFLAGS_WARN_ON += -H
QMAKE_CXXFLAGS_WARN_ON += -Wformat
QMAKE_CXXFLAGS_WARN_ON += -Wformat-security
QMAKE_CXXFLAGS_WARN_ON += -Wstack-protector
QMAKE_CXXFLAGS_WARN_ON += -Wfatal-errors
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-variable
QMAKE_CXXFLAGS_WARN_ON += -Wno-ignored-qualifiers
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-local-typedefs

DEPENDPATH += src
DEPENDPATH += src/json
DEPENDPATH += src/qt

INCLUDEPATH += src
INCLUDEPATH += src/json
INCLUDEPATH += src/qt
INCLUDEPATH += src/qt/plugins/mrichtexteditor
INCLUDEPATH += src/leveldb/include
INCLUDEPATH += src/leveldb/helpers
INCLUDEPATH += src/secp256k1/include
INCLUDEPATH += src/websocketapp