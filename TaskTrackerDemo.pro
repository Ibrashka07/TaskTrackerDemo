QT += core gui widgets

CONFIG += c++17 console
CONFIG -= app_bundle

TARGET = TaskTrackerDemo
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h \
    widgets.h \
    widgets.h