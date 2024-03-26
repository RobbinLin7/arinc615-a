QT       += core gui
QT       += network
QT       += xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
RC_ICONS = main.ico

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE
DEFINES += SPDLOG_COMPILED_LIB
#DEFINES += BIG_ENDIAN

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    LEDLight/ledlight.cpp \
    crc/crc.cpp \
    dir.cpp \
    deviceInfoWidget/deviceinfowidget.cpp \
    fileinfowidget.cpp \
    finddialog.cpp \
    main.cpp \
    mainwindow.cpp \
    mylog.cpp \
    odownloaddialog.cpp \
    operationInterface/configwidget.cpp \
    operationInterface/findinfowidget.cpp \
    operationInterface/informationwidget.cpp \
    operationInterface/mdownloadwidget.cpp \
    operationInterface/odownloadwidget.cpp \
    operationInterface/uploadwidget.cpp \
    paraconfigdialog.cpp \
    progressdialog.cpp \
    progresswidget.cpp \
    protocal/protocalfile.cpp \
    protocal/utility.cpp \
    successdialog.cpp \
    tftp/tftp.cpp \
    thread/informationthread.cpp \
    thread/mdownloadthread.cpp \
    thread/mythread.cpp \
    thread/odownloadthread.cpp \
    thread/statusfilercvthread.cpp \
    thread/uploadthread.cpp \
    topology/createtopologyxml.cpp
HEADERS += \
    AFDX/AfdxHead.h \
    LEDLight/ledlight.h \
    crc/crc.h \
    dir.h \
    device.h \
    deviceInfoWidget/deviceinfowidget.h \
    fileinfowidget.h \
    finddialog.h \
    globalDefine.h \
    mainwindow.h \
    mylog.h \
    odownloaddialog.h \
    operationInterface/configwidget.h \
    operationInterface/findinfowidget.h \
    operationInterface/informationwidget.h \
    operationInterface/mdownloadwidget.h \
    operationInterface/odownloadwidget.h \
    operationInterface/uploadwidget.h \
    paraconfigdialog.h \
    progressdialog.h \
    progresswidget.h \
    protocal/arinc615_global_var_define.h \
    protocal/arinc615_struct_define.h \
    protocal/file_define.h \
    protocal/findRequest.h \
    protocal/protocalfile.h \
    protocal/utility.h \
    singleton/singleton.h \
    successdialog.h \
    tftp/tftp.h \
    tftp/tftprequest.h \
    thread/informationthread.h \
    thread/mdownloadthread.h \
    thread/mythread.h \
    thread/odownloadthread.h \
    thread/statusfilercvthread.h \
    thread/uploadthread.h \
    topology/createtopologyxml.h
FORMS += \
    deviceInfoWidget/deviceinfowidget.ui \
    fileinfowidget.ui \
    finddialog.ui \
    mainwindow.ui \
    odownloaddialog.ui \
    operationInterface/configwidget.ui \
    operationInterface/findinfowidget.ui \
    operationInterface/informationwidget.ui \
    operationInterface/mdownloadwidget.ui \
    operationInterface/odownloadwidget.ui \
    operationInterface/uploadwidget.ui \
    paraconfigdialog.ui \
    progressdialog.ui \
    progresswidget.ui \
    successdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

LIBS += -L$$PWD/spdlog/build/ -lspdlog

INCLUDEPATH += $$PWD/spdlog/include
DEPENDPATH += $$PWD/spdlog/include
PRE_TARGETDEPS += $$PWD/spdlog/build/libspdlog.a

DISTFILES += \
    Image/Information.png \
    Image/download1.png \
    Image/download2.png \
    Image/find.png \
    Image/setting.png \
    Image/upload.png \
    arinc615A/arinc615_socket.dll \
    arinc615A/arinc615_socket.lib \
    fileInfoWidget/新建文本文档.txt \
    fileInfoWidget/新建文本文档.txt \
    spdlog/fmt/bundled/fmt.license.rst \
    tftp/readme.txt

RESOURCES += \
    icons.qrc
