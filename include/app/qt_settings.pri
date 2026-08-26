CONFIG += object_parallel_to_source
CONFIG += no_include_pwd
CONFIG += thread
CONFIG += widgets
CONFIG += openssl
CONFIG += static
CONFIG += lrelease
CONFIG -= embed_translations
CONFIG -= debug_and_release

CODECFORTR = UTF-8

QT += core
QT += gui
QT += widgets
QT += network
# v2.0.0.9 Qt6: printsupport REMOVED.  Verified 2026-08-07 that ZERO files
# in src/qt/ reference QPrint* -- carrying an unused module into a STATIC
# link is exactly the kind of thing that produces mysterious link errors.
# Re-add only if a print feature is ever actually implemented.

DEFINES += QT_GUI
DEFINES += QT_NO_VERSION_TAGGING