#ifndef UPLOADTHREAD_H
#define UPLOADTHREAD_H
#include <QThread>
#include <QRunnable>
#include <QHostAddress>
#include <QUdpSocket>
#include <cstring>
#include <condition_variable>
#include <mutex>
#include "globalDefine.h"
#include "device.h"
#include "mythread.h"
using namespace GlobalDefine;
class UploadThread : public MyThread
{
    Q_OBJECT
public:
    UploadThread(QStringList fileList, const Device* device, TftpRequest* tftpRequest, QObject *parent = 0):
        MyThread(device, tftpRequest, parent), fileList(fileList){
        status = INITIALIZATION;
        memset(&LUS, 0, sizeof(LUS));
    }
    void run() override;
    void makeLUR();
    void makeLUH();
    static File_LUS* parseLUS(QFile*);
    enum status_set{INITIALIZATION, LIST_TRANSFER, TRANSFER, END, ERROR_} status;

signals:
    void uploadResult(bool);
    void uploadStatusMessage(QString);
    void uploadRate(int, bool);
    void uploadFinish();
private:
    QStringList fileList;
    QMap<QString, bool> files_sent;
    unsigned int fileSentCnt = 0;
    unsigned int waitTimes = 0;
    File_LUS LUS;
    std::condition_variable cv;
    std::mutex m;
    volatile bool flag = false;
signals:
    void parseStatusFileFinished(File_LUS);
public slots:
    void parseStatusFile() override;
};

#endif // UPLOADTHREAD_H
