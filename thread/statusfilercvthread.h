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
    StatusFileRcvThread(TftpRequest* tftpRequest, const Device* device = nullptr, QObject* parent = nullptr)
        :MyThread(device, tftpRequest, parent){
        this->tftpRequest = tftpRequest;
    }
    ~StatusFileRcvThread();
public:
    void run();
private:
    TftpRequest* tftpRequest;
    bool error = false;
    void handleLNS();
    void handleLUS();
signals:
    void statusFileRcvFinishedSignal();

    // MyThread interface
public slots:
    void parseStatusFile();
};

#endif // STATUSFILERCVTHREAD_H
