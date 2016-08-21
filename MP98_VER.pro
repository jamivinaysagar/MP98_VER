#-------------------------------------------------
#
# Project created by QtCreator 2013-08-24T20:25:47
#
#-------------------------------------------------

QT       += core gui
QMAKE_CXXFLAGS_RELEASE += -fno-inline


greaterThan(QT_MAJOR_VER, 4): QT += widgets

TARGET = MP98_VER
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    camera.cpp \
    edit_profile.cpp \
    gpio.cpp \
    login.cpp \
    movie_disp.cpp \
    myinputpanel.cpp \
    myinputpanelcontext.cpp \
    profile.cpp \
    qcustomplot.cpp \
    temp_ctrl.cpp \
    logoff.cpp

HEADERS  += mainwindow.h \
    camera.h \
    edit_profile.h \
    gpio.h \
    login.h \
    movie_disp.h \
    myinputpanel.h \
    myinputpanelcontext.h \
    profile.h \
    qcustomplot.h \
    temp_ctrl.h \
    logoff.h

FORMS    += \
    edit_profile.ui \
    login.ui \
    movie_disp.ui \
    myinputpanelform.ui \
    mainwindow.ui \
    logoff.ui



unix:!macx:!symbian: LIBS += -L$$PWD/../../../install/ffmpeg/usr/lib/ -lavcodec

INCLUDEPATH += $$PWD/../../../install/ffmpeg/usr/include
DEPENDPATH += $$PWD/../../../install/ffmpeg/usr/include

unix:!macx:!symbian: LIBS += -L$$PWD/../../../install/ffmpeg/usr/lib/ -lavdevice

INCLUDEPATH += $$PWD/../../../install/ffmpeg/usr/include
DEPENDPATH += $$PWD/../../../install/ffmpeg/usr/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../install/ffmpeg/usr/lib/release/ -lavfilter
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../install/ffmpeg/usr/lib/debug/ -lavfilter
else:symbian: LIBS += -lavfilter
else:unix: LIBS += -L$$PWD/../../../install/ffmpeg/usr/lib/ -lavfilter

INCLUDEPATH += $$PWD/../../../install/ffmpeg/usr/include
DEPENDPATH += $$PWD/../../../install/ffmpeg/usr/include

unix:!macx:!symbian: LIBS += -L$$PWD/../../../install/ffmpeg/usr/lib/ -lavformat

INCLUDEPATH += $$PWD/../../../install/ffmpeg/usr/include
DEPENDPATH += $$PWD/../../../install/ffmpeg/usr/include

unix:!macx:!symbian: LIBS += -L$$PWD/../../../install/ffmpeg/usr/lib/ -lavutil

INCLUDEPATH += $$PWD/../../../install/ffmpeg/usr/include
DEPENDPATH += $$PWD/../../../install/ffmpeg/usr/include

unix:!macx:!symbian: LIBS += -L$$PWD/../../../install/ffmpeg/usr/lib/ -lswresample

INCLUDEPATH += $$PWD/../../../install/ffmpeg/usr/include
DEPENDPATH += $$PWD/../../../install/ffmpeg/usr/include

unix:!macx:!symbian: LIBS += -L$$PWD/../../../install/ffmpeg/usr/lib/ -lswscale

INCLUDEPATH += $$PWD/../../../install/ffmpeg/usr/include
DEPENDPATH += $$PWD/../../../install/ffmpeg/usr/include


unix:!macx:!symbian: LIBS += -L$$PWD/../../../output/lib/ -lcv

INCLUDEPATH += $$PWD/../../../output/include
DEPENDPATH += $$PWD/../../../output/include

unix:!macx:!symbian: LIBS += -L$$PWD/../../../output/lib/ -lcvaux

INCLUDEPATH += $$PWD/../../../output/include
DEPENDPATH += $$PWD/../../../output/include

unix:!macx:!symbian: LIBS += -L$$PWD/../../../output/lib/ -lcxcore

INCLUDEPATH += $$PWD/../../../output/include
DEPENDPATH += $$PWD/../../../output/include

unix:!macx:!symbian: LIBS += -L$$PWD/../../../output/lib/ -lhighgui

INCLUDEPATH += $$PWD/../../../output/include
DEPENDPATH += $$PWD/../../../output/include



unix:!macx:!symbian: LIBS += -L$$PWD/../../../output/lib/ -lml

INCLUDEPATH += $$PWD/../../../output/include
DEPENDPATH += $$PWD/../../../output/include


INCLUDEPATH += /usr/include/i386-linux-gnu
INCLUDEPATH += /usr/include
INCLUDEPATH += /home/polmon/Downloads/test/linux-2.6.32.2/drivers/media/video/
