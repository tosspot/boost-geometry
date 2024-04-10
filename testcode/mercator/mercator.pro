QT += core gui widgets positioning uitools opengl

CONFIG += c++1z debug
INCLUDEPATH +=  ./  ../../../latest/libs/dmlmisc/include/dmlmisc ../../../latest/libs/dmltmsp/include /usr/local/kernel/incl
DEPENDPATH += build

MOC_DIR = build
RCC_DIR = build
OBJECTS_DIR = build
UI_DIR = build
UIM_DIR = build

QMAKE_RPATHDIR += /home/denbridge/projects/latest/lib64/
QMAKE_CFLAGS=

LIBS += -L/usr/local/kernel/lib/linux -lEcKernel5 -lX11 -L../../../latest/lib64 -ldmlmisc -ldmltmsp

SOURCES += \
    src/main.cpp \
    src/projection.cpp \


HEADERS += \
    src/projection.h \

