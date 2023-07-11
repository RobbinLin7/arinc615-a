#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>
#include <QRunnable>
#include <QUdpSocket>
#include <QtEndian>
#include <QThread>
#include <QFile>
#include <QDir>
#include <QDataStream>
#include "device.h"
#include "tftp/tftp.h"
#include "tftp/tftprequest.h"
#include "protocal/file_define.h"
#include "crc/crc.h"
#include <QThread>
#define TFTP_READ_OP_CODE 1
#define TFTP_WRITE_OP_CODE 2
class MyThread : public QObject, public QRunnable
{
    Q_OBJECT
public:
    MyThread(const Device* device, TftpRequest* tftpRequest, QObject* parent = nullptr):QObject(nullptr),
        device(device),tftpRequest(tftpRequest){
        //创建一个虚拟连接，只接受目的设备端发来的信息
        QDir tmpDir(QDir::currentPath() + "/" + device->getName() + "_" + device->getHostAddress());
        if(!tmpDir.exists()){
            qDebug() << QDir::currentPath();
            if(!tmpDir.mkpath(QDir::currentPath() + "/" + device->getName() + "_" + device->getHostAddress())) qDebug() << "文件夹创建失败";
            else qDebug() << "文件创建成功";
        }
        dir = tmpDir;
    }
    virtual ~MyThread(){
        if(tftpServer) this->tftpServer->deleteLater();
        if(tftpClient) this->tftpClient->deleteLater();
        if(tftpRequest) delete tftpRequest;
        qDebug() << "MyThread的析构函数\n";
    }
    QHostAddress getHostAddress(){
        return QHostAddress(this->device->getHostAddress());
    }
    TftpRequest* getTftpRequest(){
        return tftpRequest;
    }
    const Device* getDevice(){
        return device;
    }
protected:
    const Device* device;
    QUdpSocket* tftpClient;
    QUdpSocket* tftpServer;
    TftpRequest* tftpRequest;
    QString statusMessage;
    QString errorMessage;
    quint16 statusCode;
    QDir dir;
    bool mainThreadExitedOrNot = false;


private:
    QMutex mutex;

signals:
    void threadFinish(bool status, QString info);
    void mainThreadExitedSignal();

public slots:
    void mainThreadExited(){
        QMutexLocker locker(&mutex);
        mainThreadExitedOrNot = true;
    }
};

#endif // MYTHREAD_H
