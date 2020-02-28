#-------------------------------------------------
#
# Project created by QtCreator 2019-11-27T11:10:25
#
#-------------------------------------------------

QT       += core gui network sql charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = loseweight
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    authorize.cpp \
    register.cpp \
    login.cpp \
    fuctionwidget.cpp \
    tool_button.cpp \
    userwidget.cpp \
    collectinfowidget.cpp \
    callout/callout.cpp \
    callout/view.cpp \
    forecastwidget.cpp

HEADERS += \
        mainwindow.h \
    authorize.h \
    register.h \
    login.h \
    fuctionwidget.h \
    tool_button.h \
    userwidget.h \
    collectinfowidget.h \
    callout/callout.h \
    callout/view.h \
    forecastwidget.h

FORMS += \
    authorize.ui \
    register.ui \
    login.ui

LIBS += \
        -L$$PWD/QRencode/lib -llibqrencode

RESOURCES += \
    resource.qrc


