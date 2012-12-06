#-------------------------------------------------
#
# Project created by QtCreator 2012-10-31T11:34:16
#
#-------------------------------------------------

QT       += core gui declarative

TARGET = Relax
TEMPLATE = app


SOURCES += main.cpp\
    src/mainwindow.cpp \
    src/relaxengine.cpp \
    src/rlistmodel.cpp \
    src/rtablemodel.cpp \
    src/watcherthread.cpp

HEADERS  += \
    src/mainwindow.h \
    src/relaxengine.h \
    src/rlistmodel.h \
    src/rtablemodel.h \
    src/watcherthread.h

FORMS    += \
    UI/mainwindow.ui

OTHER_FILES += \
    QML/main.qml \
    UI/style/style.qss \
    QML/refresh.qml \
    relaxIcon.rc

RESOURCES += \
    UI/metaobj.qrc \
    QML/qmlobj.qrc

#win32:RC_FILE = relaxIcon.rc
