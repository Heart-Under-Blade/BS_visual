#-------------------------------------------------
#
# Project created by QtCreator 2017-09-21T12:11:20
#
#-------------------------------------------------

QT       += core gui charts opengl
TEMPLATE = app
CONFIG += c++11

DESTDIR = ../bin

LIBS += -lglu32 -lopengl32

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=gnu++11
QMAKE_CXXFLAGS += -march=corei7 -msse4.2

TARGET = BS_visual
TEMPLATE = app

CONFIG(release, debug|release): DEFINES += _NDEBUG
CONFIG(debug,	debug|release): DEFINES += _DEBUG

INCLUDEPATH += \
	../src \
	../src/math \
	../src/common \
	../src/particle \
	../src/geometry \
	../src/ui \
	../src/tracing

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
		../src/*.cpp \
		../src/math/*.cpp \
		../src/particle/*.cpp \
		../src/geometry/*.cpp \
		../src/ui/*.cpp \
		../src/common/*.cpp \
		../src/tracing/*.cpp

HEADERS += \
		../src/*.h \
		../src/math/*.hpp \
		../src/math/*.h \
		../src/particle/*.h \
		../src/geometry/*.h \
		../src/ui/*.h \
		../src/common/*.h \
		../src/tracing/*.h

FORMS += \
		../src/ui/*.ui \

RESOURCES += \
	icon.qrc
