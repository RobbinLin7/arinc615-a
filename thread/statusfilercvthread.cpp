#include "statusfilercvthread.h"

StatusFileRcvThread::~StatusFileRcvThread()
{
    delete  tftpRequest;
}

void StatusFileRcvThread::run()
{
    QString errorMessage;
    QFile *statusFile = nullptr;
    this->tftpClient = new QUdpSocket();
    this->tftpServer = new QUdpSocket();
    this->tftpClient->connectToHost(device->getHostAddress(), 69);
    QByteArray request = tftpRequest->getRequest(&mainThreadExitedOrNot);
    quint16 port = tftpRequest->getPort();
    QString fileName = request.mid(2).split('\0').at(0);
    QString statusMessage;
    uint16 statusCode = 0;
    unsigned short totalFileNum = 0;
    tftpServer->disconnectFromHost();
    tftpServer->connectToHost(device->getHostAddress(), port);
    tftpRequest->lockMutex();
    if(!Tftp::receiveFile(tftpServer, QString("%1/%2").arg(dir.dirName(), fileName), &errorMessage, &mainThreadExitedOrNot, Tftp::WRQ)){
        error = true;
    }
    statusFile = new QFile(QString("%1/%2").arg(dir.dirName(), fileName));
    if(!statusFile->open(QIODevice::ReadOnly)){
        errorMessage = QString("文件%1打开失败").arg(fileName);
        error = true;
    }
    else{
        QByteArray data = statusFile->readAll();
        void* dataFileStruct = nullptr;
        dataFileStruct = (statusFileType == LUS) ? (void*)UploadThread::parseLUS(data) : (void*)ODownloadThread::parseLNS(data);
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
        statusFile->close();
        free(dataFileStruct);
        dataFileStruct = nullptr;
    }
    if(statusFileType == LUS){
        emit sendLUSInfSignal(statusCode, statusMessage, error, errorMessage);
    }
    else{
        emit sendLNSInfSignal(statusCode, totalFileNum, statusMessage, error, errorMessage);
    }
    //using namespace GlobalDefine;
    emit statusMessageSignal(STATUSFILE_RECEIVE_OP_CODE, statusMessage, device ? device->getName() : "");
}

void StatusFileRcvThread::handleLNS()
{

}

void StatusFileRcvThread::handleLUS()
{

}
