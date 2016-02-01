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
    millstate.cc \
    botmills.cc

HEADERS  += mainwindow.hh \
    board.hh \
    millstate.hh \
    botmills.hh




QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE *= -O3
QMAKE_CXXFLAGS_RELEASE *= -march=native
QMAKE_LFLAGS_RELEASE -= -Wl,-O1
QMAKE_LFLAGS_RELEASE *= -O3
