#ifndef ODOWNLOADTHREAD_H
#define ODOWNLOADTHREAD_H
#include <QObject>
#include "mythread.h"
#include <QtEndian>
#include <mutex>
#include <condition_variable>

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
        status = INITIALIZATION;
        memset(&LNS, 0, sizeof(LNS));
    }
    void run() override;
    static File_LNS* parseLNS(QFile* fLNS);
    File_LNL* parseLNL(QFile* file);
    void makeLNA();
    ~ODownloadThread(){
        delete fileList;
    }
private:
    enum status_set{INITIALIZATION, LIST_TRANSFER, TRANSFER, END, ERROR} status;
    QList<QPair<QString, QString>> *fileList = new QList<QPair<QString, QString>>();
    bool fileListReadable = false;
    QStringList checkedFileList;
    QString errorMessage;
    QString statusMessage;
    unsigned short statusCode;
    int downloadFilesCount = 0;
    unsigned int waitTimes;
    unsigned short totalFileNum = 0;
    unsigned short transmitFileNum = 1;
    std::mutex mutex;
    std::condition_variable variable;
    bool initToListTransfer = false;
    File_LNS LNS;
//    File_LNL* struct_LNL;



public slots:
    void receiveCheckedFiles(QStringList checkedFileList);
    void rcvStatusCodeAndMessageSlot(quint16 statusCode, unsigned short totalFileNum, QString statusMessage, bool error, QString errorMessage);
signals:
    void sendFileList(QList<QPair<QString, QString>>*);
    void oDownloadStatusMessage(QString);
    void oDownloadRate(int, bool);
    void parseStatusFileFinished(File_LNS);

    // MyThread interface
public slots:
    void parseStatusFile() override;
};

#endif // ODOWNLOADTHREAD_H
