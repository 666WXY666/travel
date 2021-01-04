#-------------------------------------------------
#
# Project created by QtCreator 2016-03-10T12:38:36
#
#-------------------------------------------------


QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Travel
TEMPLATE = app

UI_DIR=./UI

SOURCES += main.cpp\
        widget.cpp \
    schedule.cpp \
    attribute.cpp \
    traveler.cpp \
    mapwidget.cpp

HEADERS  += widget.h \
    schedule.h \
    attribute.h \
    traveler.h \
    mapwidget.h \
    log.h

FORMS    += widget.ui

RESOURCES += resource.qrc


