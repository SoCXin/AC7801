#-------------------------------------------------
#
# Project created by QtCreator 2014-03-26T09:26:29
#
#-------------------------------------------------

QT       += core gui

TARGET = CAN_Bootloader
TEMPLATE = app

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS += -L$$PWD/lib/ -lGinkgo_Driver

DEFINES += LANGUE_EN

RESOURCES += \
    ../source/image.qrc
RC_FILE = \
    ../source/ico.rc

FORMS += \
    ../source/mainwindow_ch.ui \
    ../source/mainwindow_en.ui

OTHER_FILES += \
    ../source/ico.rc

HEADERS += \
    ../source/CANBootloader.h \
    ../source/ErrorType.h \
    ../source/mainwindow.h \
    ../source/ControlCAN.h

SOURCES += \
    ../source/main.cpp \
    ../source/mainwindow.cpp \
    ../source/CANBootloader.cpp
