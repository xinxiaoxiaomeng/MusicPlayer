QT       += core gui multimedia network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    core/audioengine.cpp \
    core/filemanager.cpp \
    core/jsonmanager.cpp \
    core/lyricparser.cpp \
    core/musiclistcontroller.cpp \
    core/musiclistmodel.cpp \
    core/networkmanager.cpp \
    core/playbackcontroller.cpp \
    core/sqlmanager.cpp \
    main.cpp    \
    utils/md5helper.cpp \
    widgets/customdelegate.cpp \
    widgets/mainwindow.cpp \
    widgets/customslider.cpp \
    widgets/musiclistview.cpp   \
    utils/stringconstants.cpp \
    widgets/volumepopup.cpp


HEADERS += \
    core/audioengine.h \
    core/filemanager.h \
    core/jsonmanager.h \
    core/lyricparser.h \
    core/musiclistcontroller.h \
    core/musiclistmodel.h \
    core/networkmanager.h \
    core/playbackcontroller.h \
    core/sqlmanager.h \
    utils/md5helper.h \
    utils/types.h \
    widgets/customdelegate.h \
    widgets/customslider.h \
    widgets/mainwindow.h \   \
    widgets/volumepopup.h \
    widgets/musiclistview.h \
    utils/stringconstants.h \


INCLUDEPATH += \
    core/       \
    widgets/    \
    utils

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

RC_ICONS = resources/icon/logo.ico

DISTFILES +=
