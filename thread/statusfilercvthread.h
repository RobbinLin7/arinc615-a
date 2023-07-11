#ifndef STATUSFILERCVTHREAD_H
#define STATUSFILERCVTHREAD_H
#include <QObject>
#include "thread/mythread.h"
#include "thread/uploadthread.h"
#include "thread/odownloadthread.h"

class StatusFileRcvThread : public MyThread
{
    Q_OBJECT
public:
    enum StatusFileType{LUS, LNS} statusFileType;
    StatusFileRcvThread(StatusFileType statusFileType, TftpRequest* tftpRequest, const Device* device = nullptr, QObject* parent = nullptr)
        :MyThread(device, tftpRequest, parent){
        this->tftpRequest = tftpRequest;
        this->statusFileType = statusFileType;
    }
    ~StatusFileRcvThread();
    // QRunnable interface
public:
    void run();


private:
    TftpRequest* tftpRequest;
    bool error = false;
    void handleLNS();
    void handleLUS();
signals:
    void sendLNSInfSignal(quint16, quint16, QString, bool, QString);
    void sendLUSInfSignal(quint16, QString, bool, QString);
    //void sendStatusCodeAndMessageSignal(quint16, QString, bool, QString);
};

#endif // STATUSFILERCVTHREAD_H
