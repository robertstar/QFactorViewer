#-------------------------------------------------
#
# Project created by QtCreator 2016-02-17T12:42:42
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET      = FACTOR
TEMPLATE    = app


SOURCES += main.cpp\
        mainwindow.cpp\
    qcustomplot.cpp \
    settings.cpp \
    udp_port.cpp

HEADERS  += mainwindow.h\
    delegate.h \
            qcustomplot.h \ \
    settings.h \
    tabstyle.h \
    udp_port.h

FORMS    += mainwindow.ui \
    settings.ui

INCLUDEPATH += /usr/local/lib/

LIBS += -lfftw3f

#QMAKE_LFLAGS += -static

RESOURCES += \
    resources.qrc
