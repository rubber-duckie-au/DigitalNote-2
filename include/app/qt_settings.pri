CONFIG += object_parallel_to_source
CONFIG += no_include_pwd
CONFIG += thread
CONFIG += widgets
CONFIG += openssl
CONFIG += static

CONFIG -= c++17
CONFIG -= c++14
CONFIG -= c++11

CODECFORTR = UTF-8

QT += core
QT += gui
QT += widgets
QT += network
QT += printsupport

DEFINES += QT_GUI
DEFINES += QT_NO_VERSION_TAGGING