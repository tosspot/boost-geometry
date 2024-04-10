QT -= gui

LIBS += -L/usr/local/kernel/lib/linux -lEcKernel5 -lX11 -L../../../latest/lib64 -ldmlmisc -ldmltmsp
INCLUDEPATH +=  ./  ../../../latest/libs/dmlmisc/include/dmlmisc ../../../latest/libs/dmltmsp/include /usr/local/kernel/incl

CONFIG += c++17 console

SOURCES += main.cpp
QMAKE_RPATHDIR += /home/denbridge/projects/latest/lib64/


