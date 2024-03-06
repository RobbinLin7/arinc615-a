#include "statusfilercvthread.h"

StatusFileRcvThread::~StatusFileRcvThread()
{
    delete  tftpRequest;
}

void StatusFileRcvThread::run()
{
    QString errorMessage;
    QFile *statusFile = nullptr;
//    this->tftpClient = new QUdpSocket();
//    this->tftpServer = new QUdpSocket();
//    this->tftpClient->connectToHost(device->getHostAddress(), 69);
    //std::shared_ptr<QUdpSocket> uSock = std::make_shared<QUdpSocket>();
    statusFileSocket = std::make_shared<QUdpSocket>();
    if(statusFileSocket->bind(STATUS_FILE_PORT) == false){
        qDebug() << QString("端口号%1被占用").arg(STATUS_FILE_PORT);
        return;
    }
    QByteArray request = tftpRequest->getRequest();
    quint16 port = tftpRequest->getPort();
    QString fileName = request.mid(2).split('\0').at(0);
    QString statusMessage;
    uint16 statusCode;
    unsigned short totalFileNum;
    //tftpServer->disconnectFromHost();
    //tftpServer->connectToHost(device->getHostAddress(), port);
    //tftpRequest->getRequest();
    qDebug() << "start to handlePut status file";
    if(!Tftp::handlePut(statusFileSocket.get(), dir.dirName(), fileName, &errorMessage, QHostAddress(device->getHostAddress()), port, request)){
        error = true;
    }
//    if(!Tftp::receiveFile(tftpServer, QString("%1/%2").arg(dir.dirName(), fileName), &errorMessage, &mainThreadExitedOrNot, Tftp::WRQ)){
//        error = true;
//    }
    statusFile = new QFile(QString("%1/%2").arg(dir.dirName(), fileName));
    void* dataFileStruct = nullptr;
    dataFileStruct = (statusFileType == LUS) ? (void*)UploadThread::parseLUS(statusFile) : (void*)ODownloadThread::parseLNS(statusFile);
    //File_LUS *LUS_struct = UploadThread::parseLUS(data);
    if(statusFileType == LUS){
        statusMessage = QString("设备%1状态信息:%2").arg(device->getName(),
                                                   QString(((File_LUS*) dataFileStruct)->stat_des));
        statusCode = ((File_LUS*) dataFileStruct)->op_stat_code;
    }
    else{
        statusMessage = QString("设备%1状态信息:%2").arg(device->getName(),
                                                   QString(((File_LNS*) dataFileStruct)->stat_des));
        statusCode = ((File_LNS*) dataFileStruct)->op_stat_code;

        totalFileNum = ((File_LNS*) dataFileStruct)->file_num;
    }
    //free(dataFileStruct);
    //dataFileStruct = nullptr;
    if(statusFileType == LUS){
        emit(sendLUSInfSignal((File_LUS*)dataFileStruct));
    }
    else{
        emit(sendLNSInfSignal(statusCode, totalFileNum, statusMessage, error, errorMessage));
    }
    statusFileSocket->close();
}



void StatusFileRcvThread::handleLNS()
{

}

void StatusFileRcvThread::handleLUS()
{

}
