#ifndef INFORMATIONTHREAD_H
#define INFORMATIONTHREAD_H
#include <QThread>
#include <QRunnable>
#include <QHostAddress>
#include <QUdpSocket>
#include "globalDefine.h"
#include "device.h"
#include "mythread.h"
#include "spdlog/sinks/rotating_file_sink.h"
using namespace GlobalDefine;
class InformationThread : public MyThread
{
    Q_OBJECT
public:
    InformationThread(const Device* device, TftpRequest* tftpRequest, QObject* parent = nullptr):
        MyThread(device, tftpRequest, parent){
        status = SEND_LCI_RRQ;
        memset(&LCS, 0, sizeof(LCS));
    }
    void run() override;
    File_LCL* parseLCL();
private:
    enum status_set{SEND_LCI_RRQ, WAIT_LCL_WRQ, END, ERROR_} status;
    File_LCS LCS;

signals:
    void informationFinished(File_LCL* LCL_struct, QString, QString);
    void informationStatusMessage(QString message);

public slots:
    void parseStatusFile() override;
};

#endif // INFORMATIONTHREAD_H
