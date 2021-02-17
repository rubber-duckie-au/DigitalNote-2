# workaround for boost 1.58
DEFINES += BOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT
DEFINES += BOOST_BIND_GLOBAL_PLACEHOLDERS
DEFINES += BOOST_ALLOW_DEPRECATED_HEADERS

isEmpty(BOOST_THREAD_LIB_SUFFIX) {
    BOOST_THREAD_LIB_SUFFIX = $$BOOST_LIB_SUFFIX
}


# At least qmake's win32-g++-cross profile is missing the -lmingwthrd
# thread-safety flag. GCC has -mthreads to enable this, but it doesn't
# work with static linking. -lmingwthrd must come BEFORE -lmingw, so
# it is prepended to QMAKE_LIBS_QT_ENTRY.
# It can be turned off with MINGW_THREAD_BUGFIX=0, just in case it causes
# any problems on some untested qmake profile now or in the future.

windows:!contains(MINGW_THREAD_BUGFIX, 0) {
    DEFINES += _MT BOOST_THREAD_PROVIDES_GENERIC_SHARED_MUTEX_ON_WIN
    QMAKE_LIBS_QT_ENTRY = -lmingwthrd $$QMAKE_LIBS_QT_ENTRY
}