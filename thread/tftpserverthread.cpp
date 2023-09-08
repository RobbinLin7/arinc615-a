#include "tftpserverthread.h"

TftpServerThread::TftpServerThread(QObject *parent) : QThread(parent)
{
    this->moveToThread(this);
}

void TftpServerThread::run()
{
    tftpServer = new QUdpSocket(this);
    tftpServer->bind(QHostAddress::AnyIPv4, 8888, QAbstractSocket::ShareAddress);
    connect(tftpServer, &QUdpSocket::readyRead, this, &TftpServerThread::onTftpServerReadyRead);
    this->exec();
}

void TftpServerThread::onTftpServerReadyRead()
{
    qRegisterMetaType<QHostAddress>("QHostAddress");
    while(tftpServer->hasPendingDatagrams()){
        QHostAddress remote;
        quint16 port;
        QByteArray tftpRequest;
        QString fileName;
        tftpRequest.resize(tftpServer->pendingDatagramSize());
        //在每个数据包前插入两字节的端口号
        tftpServer->readDatagram(tftpRequest.data(), tftpRequest.size(), &remote, &port);
        fileName = tftpRequest.mid(2).split('\0').at(0);
        emit(requestReceivedSignal(tftpRequest, remote, port));
    }
}

void TftpServerThread::onRcvThreadExitSig()
{
    this->exit();
}
