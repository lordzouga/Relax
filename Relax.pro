#-------------------------------------------------
#
# Project created by QtCreator 2012-10-31T11:34:16
#
#-------------------------------------------------

QT       += core gui declarative

TARGET = Relax
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    relaxengine.cpp \
    rtablemodel.cpp \
    rlistmodel.cpp \
    watcherthread.cpp

HEADERS  += mainwindow.h \
    relaxengine.h \
    rtablemodel.h \
    rlistmodel.h \
    watcherthread.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    main.qml
