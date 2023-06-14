#ifndef INFORMATIONTHREAD_H
#define INFORMATIONTHREAD_H
#include <QThread>
#include <QRunnable>
#include <QHostAddress>
#include <QUdpSocket>
#include "globalDefine.h"
#include "device.h"
#include "mythread.h"
using namespace GlobalDefine;
class InformationThread : public MyThread
{
    Q_OBJECT
public:
    InformationThread(const Device* device, TftpRequest* tftpRequest, QObject* parent = nullptr):
        MyThread(device, tftpRequest, parent){
        status = SEND_LCI_RRQ;
    }
    void run() override;
    File_LCL* parseLCL();
private:
    enum status_set{SEND_LCI_RRQ, WAIT_LCL_WRQ, END, ERROR} status;

signals:
    void informationFinished(File_LCL* LCL_struct);
    void informationStatusMessage(QString message);
};

#endif // INFORMATIONTHREAD_H
