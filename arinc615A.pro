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
    spdlog/async.h \
    spdlog/async_logger-inl.h \
    spdlog/async_logger.h \
    spdlog/cfg/argv.h \
    spdlog/cfg/env.h \
    spdlog/cfg/helpers-inl.h \
    spdlog/cfg/helpers.h \
    spdlog/common-inl.h \
    spdlog/common.h \
    spdlog/details/backtracer-inl.h \
    spdlog/details/backtracer.h \
    spdlog/details/circular_q.h \
    spdlog/details/console_globals.h \
    spdlog/details/file_helper-inl.h \
    spdlog/details/file_helper.h \
    spdlog/details/fmt_helper.h \
    spdlog/details/log_msg-inl.h \
    spdlog/details/log_msg.h \
    spdlog/details/log_msg_buffer-inl.h \
    spdlog/details/log_msg_buffer.h \
    spdlog/details/mpmc_blocking_q.h \
    spdlog/details/null_mutex.h \
    spdlog/details/os-inl.h \
    spdlog/details/os.h \
    spdlog/details/periodic_worker-inl.h \
    spdlog/details/periodic_worker.h \
    spdlog/details/registry-inl.h \
    spdlog/details/registry.h \
    spdlog/details/synchronous_factory.h \
    spdlog/details/tcp_client-windows.h \
    spdlog/details/tcp_client.h \
    spdlog/details/thread_pool-inl.h \
    spdlog/details/thread_pool.h \
    spdlog/details/udp_client-windows.h \
    spdlog/details/udp_client.h \
    spdlog/details/windows_include.h \
    spdlog/fmt/bin_to_hex.h \
    spdlog/fmt/bundled/args.h \
    spdlog/fmt/bundled/chrono.h \
    spdlog/fmt/bundled/color.h \
    spdlog/fmt/bundled/compile.h \
    spdlog/fmt/bundled/core.h \
    spdlog/fmt/bundled/format-inl.h \
    spdlog/fmt/bundled/format.h \
    spdlog/fmt/bundled/locale.h \
    spdlog/fmt/bundled/os.h \
    spdlog/fmt/bundled/ostream.h \
    spdlog/fmt/bundled/printf.h \
    spdlog/fmt/bundled/ranges.h \
    spdlog/fmt/bundled/std.h \
    spdlog/fmt/bundled/xchar.h \
    spdlog/fmt/chrono.h \
    spdlog/fmt/compile.h \
    spdlog/fmt/fmt.h \
    spdlog/fmt/ostr.h \
    spdlog/fmt/ranges.h \
    spdlog/fmt/std.h \
    spdlog/fmt/xchar.h \
    spdlog/formatter.h \
    spdlog/fwd.h \
    spdlog/logger-inl.h \
    spdlog/logger.h \
    spdlog/pattern_formatter-inl.h \
    spdlog/pattern_formatter.h \
    spdlog/sinks/android_sink.h \
    spdlog/sinks/ansicolor_sink-inl.h \
    spdlog/sinks/ansicolor_sink.h \
    spdlog/sinks/base_sink-inl.h \
    spdlog/sinks/base_sink.h \
    spdlog/sinks/basic_file_sink-inl.h \
    spdlog/sinks/basic_file_sink.h \
    spdlog/sinks/callback_sink.h \
    spdlog/sinks/daily_file_sink.h \
    spdlog/sinks/dist_sink.h \
    spdlog/sinks/dup_filter_sink.h \
    spdlog/sinks/hourly_file_sink.h \
    spdlog/sinks/kafka_sink.h \
    spdlog/sinks/mongo_sink.h \
    spdlog/sinks/msvc_sink.h \
    spdlog/sinks/null_sink.h \
    spdlog/sinks/ostream_sink.h \
    spdlog/sinks/qt_sinks.h \
    spdlog/sinks/ringbuffer_sink.h \
    spdlog/sinks/rotating_file_sink-inl.h \
    spdlog/sinks/rotating_file_sink.h \
    spdlog/sinks/sink-inl.h \
    spdlog/sinks/sink.h \
    spdlog/sinks/stdout_color_sinks-inl.h \
    spdlog/sinks/stdout_color_sinks.h \
    spdlog/sinks/stdout_sinks-inl.h \
    spdlog/sinks/stdout_sinks.h \
    spdlog/sinks/syslog_sink.h \
    spdlog/sinks/systemd_sink.h \
    spdlog/sinks/tcp_sink.h \
    spdlog/sinks/udp_sink.h \
    spdlog/sinks/win_eventlog_sink.h \
    spdlog/sinks/wincolor_sink-inl.h \
    spdlog/sinks/wincolor_sink.h \
    spdlog/spdlog-inl.h \
    spdlog/spdlog.h \
    spdlog/stopwatch.h \
    spdlog/tweakme.h \
    spdlog/version.h \
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
