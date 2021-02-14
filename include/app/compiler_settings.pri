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

LIBS += -Lsrc/leveldb
LIBS += -lleveldb
LIBS += -lmemenv

LIBS += src/secp256k1/.libs/libsecp256k1.a

LIBS += -lcrypto
LIBS += -lboost_system$$BOOST_LIB_SUFFIX
LIBS += -lboost_filesystem$$BOOST_LIB_SUFFIX
LIBS += -lboost_program_options$$BOOST_LIB_SUFFIX
LIBS += -lboost_thread$$BOOST_THREAD_LIB_SUFFIX
LIBS += -lboost_chrono$$BOOST_LIB_SUFFIX
LIBS += -ldb_cxx$$BDB_LIB_SUFFIX
LIBS += -ldl
LIBS += -levent
LIBS += -lgmp
LIBS += -lrt
LIBS += -lssl
LIBS += -lz
LIBS += $$join(BOOST_LIB_PATH,,-L,)
LIBS += $$join(BDB_LIB_PATH,,-L,)
LIBS += $$join(OPENSSL_LIB_PATH,,-L,)
LIBS += $$join(QRENCODE_LIB_PATH,,-L,)

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
INCLUDEPATH += $$BOOST_INCLUDE_PATH $$BDB_INCLUDE_PATH $$OPENSSL_INCLUDE_PATH $$QRENCODE_INCLUDE_PATH

DEFINES += LINUX
DEFINES += ENABLE_WALLET
DEFINES += QT_GUI
DEFINES += BOOST_THREAD_USE_LIB
DEFINES += BOOST_SPIRIT_THREADSAFE