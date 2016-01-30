#-------------------------------------------------
#
# Project created by QtCreator 2016-01-30T10:36:52
#
#-------------------------------------------------

QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mills
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cc\
        mainwindow.cc \
    board.cc \
    millstate.cc

HEADERS  += mainwindow.hh \
    board.hh \
    millstate.hh
