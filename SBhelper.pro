#-------------------------------------------------
#
# Project created by QtCreator 2019-08-27T08:46:35
#
#-------------------------------------------------
include (src/qcustomplot/qcustomplot.pri)

QT       +=  core gui serialport bluetooth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SBhelper
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++14

SOURCES += \
        src/dialogskin.cpp \
        src/iodevice.cpp \
        src/main.cpp \
        src/mainwindow.cpp \
        src/multicurvesplot.cpp \
        src/multicurvesplotprivate.cpp \
        src/rudder.cpp \
        src/serialsettingsdialog.cpp \
        src/widgetloading.cpp \
        src/widgetpainter.cpp \
        src/widgetparaitem.cpp\
        src/chatclient.cpp

HEADERS += \
        src/dialogskin.h \
        src/iodevice.h \
        src/mainwindow.h \
        src/multicurvesplot.h \
        src/multicurvesplotprivate.h \
        src/rudder.h \
        src/serialsettingsdialog.h \
#        src/uartconfigdialog.h \
        src/widgetloading.h \
        src/widgetpainter.h \
        src/widgetparaitem.h\
        src/chatclient.h

FORMS += \
        gui/dialogskin.ui \
        gui/mainwindow.ui \
        gui/serialsettingsdialog.ui \
        gui/widgetparaitem.ui \
        gui/widgetloading.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res/res.qrc
