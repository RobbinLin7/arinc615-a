#include "statusfilercvthread.h"

StatusFileRcvThread::~StatusFileRcvThread()
{
    delete  tftpRequest;
}

void StatusFileRcvThread::run()
{
    QString errorMessage;
    statusFileSocket = std::make_shared<QUdpSocket>();
    if(statusFileSocket->bind(STATUS_FILE_PORT) == false){
        qDebug() << QString("端口号%1被占用").arg(STATUS_FILE_PORT);
        return;
    }
    QByteArray request = tftpRequest->getRequest();
    quint16 port = tftpRequest->getPort();
    QString fileName = request.mid(2).split('\0').at(0);
    QString statusMessage;
    const unsigned short DLP_retry = 2;
    unsigned short tries = 0;
    while(!Tftp::handlePut(statusFileSocket.get(), dir.dirName(), fileName, &errorMessage, QHostAddress(device->getHostAddress()), port, request)
          && ++tries < DLP_retry + 1){

    }
    if(tries >= DLP_retry + 1){
        qDebug() << "超过DLP重传次数" << QString("状态文件%1获取失败").arg(fileName);
    }
    else emit statusFileRcvFinishedSignal();
    statusFileSocket->close();

}



void StatusFileRcvThread::handleLNS()
{

}

void StatusFileRcvThread::handleLUS()
{

}

void StatusFileRcvThread::parseStatusFile()
{

}
