QT += core gui widgets testlib
TEMPLATE = app
TARGET = hmi-ui-tests
CONFIG += c++11 testcase console
CONFIG -= app_bundle
msvc: QMAKE_CXXFLAGS += /utf-8
include(../hmi_sources.pri)
SOURCES += ui_tests.cpp
RESOURCES += ../assets/hmi.qrc
