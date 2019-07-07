QT += testlib
QT -= gui

################################################################################

CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

################################################################################

TARGET = test_f16_aero_data

################################################################################

DEFINES += QT_DEPRECATED_WARNINGS

################################################################################

INCLUDEPATH += . ..

win32: INCLUDEPATH += \
    $(OSG_ROOT)/include/ \
    $(OSG_ROOT)/include/libxml2

unix: INCLUDEPATH += \
    /usr/include/libxml2

################################################################################

win32: LIBS += \
    -L$(OSG_ROOT)/lib \
    -llibxml2

unix: LIBS += \
    -L/lib \
    -L/usr/lib \
    -lxml2

################################################################################

include(../fdm/fdm.pri)
include(../fdm_c130/fdm_c130.pri)
include(../fdm_c172/fdm_c172.pri)
include(../fdm_f16/fdm_f16.pri)
include(../fdm_uh60/fdm_uh60.pri)

################################################################################

SOURCES += \
    test_f16_aero_data.cpp

################################################################################

DEFINES += SRCDIR=\\\"$$PWD/\\\"