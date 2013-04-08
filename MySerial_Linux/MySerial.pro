#-------------------------------------------------
#
# Project created by QtCreator 2013-02-16T17:00:43
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MySerial
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qextserialbase.cpp \
    posix_qextserialport.cpp \
    readport.cpp

HEADERS  += mainwindow.h \
    posix_qextserialport.h \
    readport.h \
    qextserialbase.h

FORMS    += mainwindow.ui
