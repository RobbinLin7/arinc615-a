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
    QStringList fileList;
    QString errorMessage;
    enum status_set{SEND_LND_RRQ, WAIT_LNS_WRQ, SEND_LNR_WRQ, WAIT_FILE_WRQ, END, ERROR} status;

signals:
    void mDownloadStatusMessage(QString);
    void mDownloadRate(int, bool);
};

#endif // MDOWNLOADTHREAD_H
