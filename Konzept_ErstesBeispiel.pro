#-------------------------------------------------
#
# Project created by QtCreator 2019-03-18T14:52:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Konzept_ErstesBeispiel
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

CONFIG += c++11

SOURCES += \
        main.cpp \
    model.cpp \
    controller.cpp \
    poller.cpp \
    eventqueue.cpp \
    serverwatchdog.cpp \
    bbmjoystick.cpp \
    networkinterface.cpp \
    telegrambuilder.cpp \
    errorwindow.cpp \
    udplistener.cpp \
    rotary.cpp \
    tastenfeld.cpp \
    smbus.cpp \
    ocp.cpp \
    home.cpp \
    view.cpp \
    cameraview.cpp \
    others.cpp \
    generictimer.cpp \
    xptcontrol.cpp \
    xptinterface.cpp


HEADERS += \
    model.h \
    controller.h \
    poller.h \
    events.h \
    eventqueue.h \
    serverwatchdog.h \
    bbmjoystick.h \
    bbmcommandtypes.h \
    networkinterface.h \
    telegrambuilder.h \
    errorwindow.h \
    udplistener.h \
    rotary.h \
    tastenfeld.h \
    config.h \
    smbus.h \
    ocp.h \
    home.h \
    view.h \
    cameraview.h \
    others.h \
    generictimer.h \
    xptcontrol.h \
    xptinterface.h

FORMS += \
    errorwindow.ui \
    home.ui \
    view.ui \
    cameraview.ui \
    others.ui \
    xptcontrol.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    styles.qrc

DISTFILES += \
    stylesheet.qss
