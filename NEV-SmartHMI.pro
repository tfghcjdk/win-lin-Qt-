QT += core gui widgets
TEMPLATE = app
TARGET = NEV-SmartHMI
CONFIG += c++11
DEFINES += QT_DEPRECATED_WARNINGS
msvc: QMAKE_CXXFLAGS += /utf-8
include(hmi_sources.pri)
SOURCES += src/main.cpp
FORMS += src/ui/main_window.ui
RESOURCES += assets/hmi.qrc
DISTFILES += README.md PRD.md TECH_SPEC.md CHANGELOG.md assets/qss/dark_theme.qss config/camera_calibration.ini.example
