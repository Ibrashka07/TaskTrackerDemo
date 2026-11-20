QT += core gui widgets

CONFIG += c++17
# На macOS console может мешать открытию окна, но для логов оставь как есть
# CONFIG += console
CONFIG -= app_bundle

TARGET = TaskTrackerDemo
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h \
    widgets.h