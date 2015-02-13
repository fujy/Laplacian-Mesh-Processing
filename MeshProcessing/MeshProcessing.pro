#-------------------------------------------------
#
# Project created by QtCreator 2015-01-14T01:16:25
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MeshProcessing
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    model/vertex.cpp \
    model/mesh.cpp \
    view/glwidget.cpp \
    model/meshbuilder.cpp \
    model/face.cpp \
    view/grid.cpp \
    view/lightsource.cpp

HEADERS  += mainwindow.h \
    model/vertex.h \
    model/mesh.h \
    view/glwidget.h \
    model/meshbuilder.h \
    model/face.h \
    constants.h \
    view/grid.h \
    view/lightsource.h

FORMS    += mainwindow.ui

INCLUDEPATH += lib

RESOURCES += \
    resources.qrc
