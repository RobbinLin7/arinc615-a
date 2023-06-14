#ifndef UPLOADTHREAD_H
#define UPLOADTHREAD_H
#include <QThread>
#include <QRunnable>
#include <QHostAddress>
#include <QUdpSocket>
#include "globalDefine.h"
#include "device.h"
#include "mythread.h"
using namespace GlobalDefine;
class UploadThread : public MyThread
{
    Q_OBJECT
public:
    UploadThread(QStringList fileList, const Device* device, TftpRequest* tftpRequest, bool subOfAuto = false, QObject *parent = 0):
        MyThread(device, tftpRequest, parent), fileList(fileList){
        status = SEND_LUI_RRQ;
        this->subOfAuto = subOfAuto;
    }
    void run() override;
    void makeLUR();
    void makeLUH();
    File_LUS* parseLUS(QByteArray);
signals:
    void uploadResult(bool);
    void uploadStatusMessage(QString);
    void uploadRate(int, bool);
    void uploadFinish();
private:
    QStringList fileList;
    enum status_set{SEND_LUI_RRQ, WAIT_LUS_WRQ, SEND_LUR_WRQ, WAIT_LUH_RRQ, WAIT_FILE_RRQ, END, ERROR} status;
    QMap<QString, bool> files_sent;
    unsigned int fileSentCnt = 0;
    bool subOfAuto;
public slots:
    void mainThreadExited(){
        mainThreadExitedOrNot = true;
        qDebug() << "主线程已经退出了";
    }
};

#endif // UPLOADTHREAD_H
