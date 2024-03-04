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
    static File_LUS* parseLUS(QFile*);
    enum status_set{SEND_LUI_RRQ, SEND_LUR_WRQ, WAIT_LUH_RRQ, WAIT_FILE_RRQ, END, ERROR} status;
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
    bool subOfAuto;
public slots:
    void mainThreadExited(){
        mainThreadExitedOrNot = true;
    }
    void rcvStatusCodeAndMessageSlot(quint16 statusCode, QString statusMessage, bool error, QString errorMessage);
};

#endif // UPLOADTHREAD_H
