QT       += core gui widgets concurrent

CONFIG += c++14


SOURCES += \
    main.cpp \
    mainwindow.cpp \
    ping_tree_model.cpp

HEADERS += \
    mainwindow.h \
    ping_tree_model.h

FORMS += \
    mainwindow.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
