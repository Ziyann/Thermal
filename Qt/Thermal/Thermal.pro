#-------------------------------------------------
#
# Project created by QtCreator 2015-11-03T14:02:48
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Thermal
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    setupdialog.cpp \
    thermometerdialog.cpp \
    worker.cpp \
    controller.cpp \
    thermaldata.cpp

HEADERS  += mainwindow.h \
    setupdialog.h \
    thermometerdialog.h \
    worker.h \
    controller.h \
    commands.h \
    thermaldata.h

FORMS    += mainwindow.ui \
    setupdialog.ui \
    thermometerdialog.ui

DISTFILES +=
