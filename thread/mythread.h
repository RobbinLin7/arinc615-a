#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>
#include <QRunnable>
#include <QUdpSocket>
#include <QtEndian>
#include <QThread>
#include <QFile>
#include <QDir>
#include <QDataStream>
#include <QWaitCondition>
#include "device.h"
#include "tftp/tftp.h"
#include "tftp/tftprequest.h"
#include "protocal/file_define.h"
#include "crc/crc.h"
#include <QThread>
#include "singleton/singleton.h"
#include "mylog.h"
#include "spdlog/spdlog.h"

#define TFTP_READ_OP_CODE 1
#define TFTP_WRITE_OP_CODE 2
class MyThread : public QObject, public QRunnable
{
    Q_OBJECT
public:
    MyThread(const Device* device, TftpRequest* tftpRequest, QObject* parent = nullptr);
    virtual ~MyThread();
    const QHostAddress getHostAddress() const;
    TftpRequest* getTftpRequest() const;
    const Device *getDevice() const;
protected:
    const Device* device;
    std::shared_ptr<QUdpSocket> protocalFileSocket;
    std::shared_ptr<QUdpSocket> statusFileSocket;
    spdlog::sinks_init_list list;
    spdlog::logger& logger{Singleton<spdlog::logger>::Instance("logger", list)};
    TftpRequest* tftpRequest;
    QString statusMessage;
    QString errorMessage;
    quint16 statusCode;
    QDir dir;
    bool mainThreadExitedOrNot = false;
    bool statusFileRcved = false;
    QMutex conditionMutex;
    QWaitCondition statusFileRcvedConditon;
    void waitStatusFileRcved();
    bool waitStatusFileRcved(QString& errorMessage, unsigned long mseconds = ULONG_MAX);
private:
    QMutex mutex;
signals:
    void threadFinish(bool status, QString info);
    void mainThreadExitedSignal();
public slots:
    void mainThreadExited();
    virtual void parseStatusFile() = 0;
};

#endif // MYTHREAD_H
