#ifndef ODOWNLOADTHREAD_H
#define ODOWNLOADTHREAD_H
#include <QObject>
#include "mythread.h"
#include <QtEndian>

class ODownloadThread : public MyThread
{
    Q_OBJECT
public:
    explicit ODownloadThread(TftpRequest* tftpRequest, const Device* device, QStringList fileList = QStringList(), QObject *parent = nullptr)
        :MyThread(device, tftpRequest, parent){
        if(fileList.size() > 0){
            checkedFileList = fileList;
            this->fileListReadable = true;
        }
        status = SEND_LNO_RRQ;
    }
    void run() override;
    static File_LNS* parseLNS(QFile* fLNS);
    File_LNL* parseLNL(const QByteArray data);
    void makeLNA();
    ~ODownloadThread(){
        delete fileList;
    }
private:
    enum status_set{SEND_LNO_RRQ, WAIT_LNS_WRQ, WAIT_LNL_WRQ, SEND_LNA_WRQ, WAIT_FILE, END, ERROR} status;
    QList<QPair<QString, QString>> *fileList;
    bool fileListReadable = false;
    QStringList checkedFileList;
    QString errorMessage;
    QString statusMessage;
    unsigned short statusCode;
    unsigned int waitTimes;
    unsigned short totalFileNum = 0;
    unsigned short transmitFileNum = 1;
public slots:
    void receiveCheckedFiles(QStringList checkedFileList);
    void rcvStatusCodeAndMessageSlot(quint16 statusCode, unsigned short totalFileNum, QString statusMessage, bool error, QString errorMessage);
signals:
    void sendFileList(QList<QPair<QString, QString>>*);
    void oDownloadStatusMessage(QString);
    void oDownloadRate(int, bool);

    // MyThread interface
public slots:
    void parseStatusFile();
};

#endif // ODOWNLOADTHREAD_H
