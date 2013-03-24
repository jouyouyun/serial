#-------------------------------------------------
#
# Project created by QtCreator 2013-02-16T17:00:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MySerial
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qextserialbase.cpp \
    posix_qextserialport.cpp \
    readport.cpp

HEADERS  += mainwindow.h \
    qextserialbase.h \
    posix_qextserialport.h \
    readport.h

FORMS    += mainwindow.ui
