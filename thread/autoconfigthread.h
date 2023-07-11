#ifndef AUTOCONFIGTHREAD_H
#define AUTOCONFIGTHREAD_H

#include <QObject>
#include <QThreadPool>
#include "mythread.h"
#include "uploadthread.h"
#include "mdownloadthread.h"
#include "odownloadthread.h"
#include <QCryptographicHash>
#include <QThreadPool>

class AutoConfigThread : public MyThread
{
    Q_OBJECT
public:
    explicit AutoConfigThread(QThreadPool *pool, QStringList filesPath, const Device* device, TftpRequest *tftpRequest, QObject *parent = nullptr)
        :MyThread(device, tftpRequest, parent)
    {
        this->filesPath = filesPath;
        this->device = device;
        this->pool = pool;
        for(QString filePath : filesPath){
            filesName.append(filePath.mid(filePath.lastIndexOf('/') + 1));
        }
        uploadThread = new UploadThread(filesPath, device, tftpRequest, true);
        oDownloadThread = new ODownloadThread(tftpRequest, device, filesName);
        connect(this, &AutoConfigThread::mainThreadExitedSignal, uploadThread, &MyThread::mainThreadExited);
        connect(this, &MyThread::mainThreadExitedSignal, oDownloadThread, &MyThread::mainThreadExited);
    }
    ~AutoConfigThread(){

    }
    void run() override;
private:
    const Device* device;
    QStringList filesPath;
    QStringList filesName;
    QThreadPool* pool;
    MyThread* uploadThread = nullptr;
    MyThread* oDownloadThread = nullptr;
    int uploadComplete = 0;
    int downloadComplete = 0;
    void startDownload();
    void startCompareMd5();
public slots:
    void mainThreadExited(){
        mainThreadExitedOrNot = true;
        emit(mainThreadExitedSignal());
    }
    void rcvLUSInfSlot(quint16 statusCode, QString statusMessage, bool error, QString errorMessage);
    void rcvLNSInfSlot(quint16 statusCode, quint16 totalFileNum, QString statusMessage, bool error, QString errorMessage);

signals:
    void autoConfigStatusMessage(QString);
    void autoConfigRate(int, bool);
    void sendLUSInfSignal(quint16 statusCode, QString statusMessage, bool error, QString errorMessage);
    void sendLNSInfSignal(quint16 statusCode, quint16 totalFileNum, QString statusMessage, bool error, QString errorMessage);
};

#endif // AUTOCONFIGTHREAD_H
