#ifndef MDOWNLOADTHREAD_H
#define MDOWNLOADTHREAD_H

#include "mythread.h"

class MDownloadThread : public MyThread
{
    Q_OBJECT
public:
    explicit MDownloadThread(QStringList fileList, const Device* device, TftpRequest* tftpRequest, QObject *parent = nullptr):
        MyThread(device, tftpRequest, parent), fileList(fileList), status(SEND_LND_RRQ){}
    void run() override;
    void makeLNR();
    File_LNS* parseLNS(const QByteArray data);
    File_LNS* parseLNS();
private:
    uint totalFileNum;
    QStringList fileList;
    unsigned int waitTimes;
    QString errorMessage;
    enum status_set{SEND_LND_RRQ, SEND_LNR_WRQ, WAIT_FILE_WRQ, END, ERROR} status;
    void modifyStatus(status_set fromStatus, status_set currentStatus);

public slots:
    void rcvStatusCodeAndMessageSlot(quint16 statusCode, unsigned short totalFileNum, QString statusMessage, bool error, QString errorMessage);
signals:
    void mDownloadStatusMessage(QString);
    void mDownloadRate(int, bool);
};

#endif // MDOWNLOADTHREAD_H
