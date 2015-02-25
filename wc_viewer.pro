#-------------------------------------------------
#
# Project created by QtCreator 2014-12-06T21:08:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


CONFIG(debug, debug|release) : TARGET = wc_viewerd
                        else : TARGET = wc_viewer
TEMPLATE = app

DESTDIR = $${PWD}/bin

SOURCES += main.cpp\
        mainwindow.cpp \
    em_parser.cpp \
    wc_ping_viewer.cpp \
    trackline_viewer.cpp

HEADERS  += mainwindow.h \
    em_datagram.h \
    em_parser.h \
    raw_parser.h \
    raw_parser_global.h \
    wc_ping_viewer.h \
    trackline_viewer.h \
    types.h

FORMS    += mainwindow.ui

#OpenMP supported
QMAKE_CXXFLAGS += -fopenmp
LIBS += -lgomp
#LIBS += -lpthread

INCLUDEPATH += D:/Qt/Qt4.8.1/OpenCV/include

LIBS += -LD:/Qt/Qt4.8.1/OpenCV/lib \
    -LD:/Qt/Qt4.8.1/OpenCV/bin \
    -lopencv_core249 \
    -lopencv_highgui249 \
    -lopencv_imgproc249 \
    -lws2_32

RESOURCES += \
    wc_viewr.qrc
