TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    fsm.cpp \
    rwhandle.cpp \
    serivice.cpp

INCLUDEPATH += C:/local/boost_1_69_0/
LIBS += -lpthread libwsock32 libws2_32

HEADERS += \
    fsm.h \
    rwhandle.h \
    serivice.h \
    threadpool.h
