#include "odownloadthread.h"


void ODownloadThread::run()
{
    this->tftpClient = new QUdpSocket();
    this->tftpServer = new QUdpSocket();
    this->tftpClient->connectToHost(device->getHostAddress(), 69);
    QByteArray request;
    quint16 port;
    QString fileName;
    QFile *LNL = nullptr;
    unsigned int waitUserTimes = 0;
    while(status != END){
        waitTimes = 0;
        switch (status) {
        case SEND_LNO_RRQ:
            if(mainThreadExitedOrNot || !Tftp::receiveFile(tftpClient, QString("%1/%2.LNO").arg(dir.dirName(), device->getName()), &errorMessage, &mainThreadExitedOrNot, Tftp::RRQ)){
                status = ERROR;
                break;
            }
            emit oDownloadStatusMessage(tr("LNO接收完成"));
            if(waitStatusFileRcved(errorMessage, max_retrans_times * max_find_response_time_ms) == false){
                status = ERROR;
                break;
            }
            break;
        case WAIT_LNL_WRQ:
            request = tftpRequest->getRequest(&mainThreadExitedOrNot);
            if(request.size() == 0){
                status = ERROR;
                if(mainThreadExitedOrNot){
                    errorMessage = QString("主进程已退出");
                }
                else{
                    errorMessage = QString("等待LNL写请求超时");
                }
                break;
            }
            port = tftpRequest->getPort();
            fileName = request.mid(2).split('\0').at(0);
            tftpServer->disconnectFromHost();
            tftpServer->connectToHost(device->getHostAddress(), port);
            tftpRequest->lockMutex();
            if(!Tftp::receiveFile(tftpServer, QString("%1/%2").arg(dir.dirName(), fileName), &errorMessage, &mainThreadExitedOrNot, Tftp::WRQ)){
                status = ERROR;
                break;
            }
            emit(oDownloadStatusMessage(QString(tr("LNL文件接收成功"))));
            LNL = new QFile(QString("%1/%2").arg(dir.dirName(), fileName));
            if(!LNL->open(QIODevice::ReadOnly)){
                status = ERROR;
                errorMessage = QString("LNL文件打开失败");
                break;
            }
            else{
                File_LNL *LNL_struct = parseLNL(LNL->readAll());
                fileList = new QList<QPair<QString, QString>>();
                if(LNL_struct->file_num == 0){
                    free(LNL_struct);
                    status = ERROR;
                    errorMessage = QString(tr("没有文件可供用户定义下载"));
                    break;
                }
                for(int i = 0; i < LNL_struct->file_num; i++){
                    QPair<QString, QString> pair;
                    pair.first = QString(LNL_struct->file[i].file_name);
                    pair.second = QString(LNL_struct->file[i].file_stat_des);
                    fileList->push_back(pair);
                }

                free(LNL_struct);
                emit sendFileList(fileList);
                emit oDownloadStatusMessage(QString(tr("LNL文件解析成功")));
                status = SEND_LNA_WRQ;
            }
            break;
        case SEND_LNA_WRQ:
            while(!mainThreadExitedOrNot && !fileListReadable && waitUserTimes < 1000){
                QThread::msleep(200);
                waitUserTimes++;
            }
            if(mainThreadExitedOrNot){
                errorMessage = QString("主线程已退出");
                status = ERROR;
                break;
            }
            if(!fileListReadable){
                status = ERROR;
                errorMessage = QString("等待用户选择超时");
                break;
            }
            emit oDownloadStatusMessage(QString(tr("获取下载文件列表成功")));
            makeLNA();
            emit oDownloadStatusMessage(QString(tr("LNA文件构造成功")));
            if(!Tftp::sendFile(tftpClient, QString("%1/%2.LNA").arg(dir.dirName(), device->getName()), &errorMessage, &mainThreadExitedOrNot, Tftp::WRQ)){
                status = ERROR;
                break;
            }
            emit oDownloadStatusMessage(QString(tr("LNA文件发送成功")));
            if(waitStatusFileRcved(errorMessage, max_retrans_times * max_find_response_time_ms) == false){
                status = ERROR;
                break;
            }
        case WAIT_FILE:{
            QByteArray request = tftpRequest->getRequest(&mainThreadExitedOrNot);
            if(request.size() == 0){
                if(mainThreadExitedOrNot){
                    errorMessage = QString("主进程已退出");
                }
                else{
                    errorMessage = QString("等待设备端TFTP写请求超时");
                }
                status = ERROR;
                break;
            }
            quint16 port = tftpRequest->getPort();
            QString fileName = request.mid(2).split('\0').at(0);
            tftpServer->disconnectFromHost();
            tftpServer->connectToHost(device->getHostAddress(), port);
            tftpRequest->lockMutex();
            if(!Tftp::receiveFile(tftpServer, QString("%1/%2").arg(dir.dirName(), fileName), &errorMessage, &mainThreadExitedOrNot,Tftp::WRQ)){
                status = ERROR;
                break;
            }
            emit oDownloadStatusMessage(QString("设备%1: 文件%2下载完成(%3/%4)").
                                        arg(device->getName(),
                                            fileName,
                                            QString::number(transmitFileNum),
                                            QString::number(totalFileNum)));
            emit oDownloadRate(100 * transmitFileNum++ / totalFileNum, true);
            if(waitStatusFileRcved(errorMessage, max_retrans_times * max_find_response_time_ms) == false){
                status = ERROR;
                break;
            }
            break;
        }
        case ERROR:
            emit oDownloadStatusMessage(QString("设备%1: %2").arg(device->getName(), errorMessage));
            emit oDownloadStatusMessage(QString("用户定义下载异常结束"));
            emit oDownloadRate(0, false);
            emit threadFinish(false, errorMessage);
            status = END;
            break;
        case END:
            break;
        default:
            break;
        }
    }
}

File_LNS *ODownloadThread::parseLNS(const QByteArray data)
{
    File_LNS *LNS = (File_LNS*)malloc(sizeof(File_LNS));
    //LNS->file_len = data[0] + ((quint32)data[1] << 8) + ((uint32)data[2] << 16) + ((uint32)data[3] << 24);
    memcpy(&LNS->file_len, data.mid(0, 4).data(), sizeof(LNS->file_len));
    //LNS->Pro_ver = data[4] + ((quint16)data[5] << 8);
    memcpy(&LNS->Pro_ver, data.mid(4, 2).data(), sizeof(LNS->Pro_ver));
    //LNS->op_stat_code = data[6] + ((quint16)data[7] << 8);
    memcpy(&LNS->op_stat_code, data.mid(6, 2).data(), sizeof(LNS->op_stat_code));
    //LNS->stat_des_len = data[8];
    memcpy(&LNS->stat_des_len, data.mid(8, 1).data(), sizeof(unsigned char));
    strcpy(LNS->stat_des, QString::fromStdString(data.mid(9, LNS->stat_des_len).toStdString()).toUtf8().data());
    size_t pos = 9 + sizeof(LNS->stat_des);
    //LNS->counter = ((quint16)data[pos] << 8) + data[pos + 1];
    memcpy(&LNS->counter, data.mid(pos, 2).data(), sizeof(LNS->counter));
    pos += 2;
    //LNS->excep_timer = ((quint16)data[pos] << 8) + data[pos + 1];
    memcpy(&LNS->excep_timer, data.mid(pos, 2).data(), sizeof(LNS->excep_timer));
    pos += 2;
    //LNS->estim_timer = ((quint16)data[pos] << 8) + data[pos + 1];
    memcpy(&LNS->estim_timer, data.mid(pos, 2).data(), sizeof(LNS->estim_timer));
    pos += 2;
    memcpy(LNS->down_list_ratio, data.mid(pos, 3).data(), 3);
    pos += 3;
    //LNS->file_num = (quint16)data[pos] + ((quint16)data[pos + 1] << 8);
    memcpy(&LNS->file_num, data.mid(pos, 2).data(), sizeof(LNS->file_num));
    pos += 2;
    LNS->LNS_files = (file_info_LNS *)malloc(sizeof(file_info_LNS) * LNS->file_num);
    for(int i = 0; i < LNS->file_num; i++){
        //LNS->LNS_files[i].file_name_len = (unsigned char)data[pos++];
        memcpy(&LNS->LNS_files[i].file_name_len, data.mid(pos, 1).data(), sizeof(unsigned char));
        pos++;
        strcpy(LNS->LNS_files[i].file_name, data.mid(pos, LNS->LNS_files[i].file_name_len).data());
        //pos += sizeof(LNS->LNS_files[i].file_name);
        pos += 255;
        //LNS->LNS_files[i].file_stat = ((quint16)data[pos] << 8) + data[pos + 1];
        memcpy(&LNS->LNS_files[i].file_stat, data.mid(pos, 2).data(), sizeof(LNS->LNS_files[i].file_stat));
        pos += 2;
        //LNS->LNS_files[i].file_stat_des_len = (unsigned char)data[pos++];
        memcpy(&LNS->LNS_files[i].file_stat_des_len, data.mid(pos, 1).data(), sizeof(unsigned char));
        pos++;
        strcpy(LNS->LNS_files[i].file_stat_des, data.mid(pos, LNS->LNS_files[i].file_stat_des_len).data());
        //pos += sizeof(LNS->LNS_files[i].file_stat_des);
        pos += 255;
    }
    return LNS;
}

File_LNL *ODownloadThread::parseLNL(const QByteArray data)
{
    File_LNL *LNL = (File_LNL*)malloc(sizeof(File_LNL));
    LNL->file_len = data[0] + ((quint32)data[1] << 8) + ((uint32)data[2] << 16) + ((uint32)data[3] << 24);
    LNL->Pro_ver = data[4] + ((quint16)data[5] << 8);
    LNL->file_num = data[6] + ((quint16)data[7] << 8);
    LNL->file = (file_info_LNL*)malloc(sizeof(file_info_LNL) * LNL->file_num);
    memset(LNL->file, 0, sizeof(file_info_LNL) * LNL->file_num);
    unsigned int pos = 8;
    for(int i = 0; i < LNL->file_num; i++){
        LNL->file[i].file_name_len = data[pos++];
        strcpy(LNL->file[i].file_name, data.mid(pos, LNL->file[i].file_name_len));
        //pos += sizeof(LNL->file[i].file_name);
        pos += 255;
        LNL->file[i].file_stat_des_len = data[pos++];
        strcpy(LNL->file[i].file_stat_des, data.mid(pos, LNL->file[i].file_stat_des_len));
        //pos += sizeof(LNL->file[i].file_stat_des);
        pos += 255;
    }
    return LNL;
}

void ODownloadThread::makeLNA()
{
    File_LNA *LNA_struct = (File_LNA*)malloc(sizeof (File_LNA));
    LNA_struct->file_len = sizeof (LNA_struct->file_len) + sizeof (LNA_struct->Pro_ver) + sizeof(LNA_struct->file_num);
    LNA_struct->file_num = checkedFileList.size();
    LNA_struct->file = (Hfile_info*)malloc(sizeof (Hfile_info) * LNA_struct->file_num);
    memcpy(LNA_struct->Pro_ver, Protocol_ver, 2);
    for(int i = 0; i < LNA_struct->file_num; i++){
        LNA_struct->file_len += sizeof(LNA_struct->file[i].len_name) + sizeof(LNA_struct->file[i].name);
        LNA_struct->file[i].len_name = checkedFileList.at(i).size();
        strcpy(LNA_struct->file[i].name, checkedFileList.at(i).toUtf8().data());
    }
    QFile LNA(QString("%1/%2.LNA").arg(dir.dirName(), device->getName()));
    if(LNA.open(QIODevice::WriteOnly)){
        LNA.write((char*)&LNA_struct->file_len, sizeof (LNA_struct->file_len));
//        qToBigEndian(LNA_struct->Pro_ver, &LNA_struct->Pro_ver);
        LNA.write(LNA_struct->Pro_ver, sizeof(LNA_struct->Pro_ver));
        LNA.write((char*)&LNA_struct->file_num, sizeof(LNA_struct->file_num));
        for(int i = 0; i < LNA_struct->file_num; i++){
            LNA.write((char*)&LNA_struct->file[i].len_name, sizeof(LNA_struct->file[i].len_name));
            LNA.write(LNA_struct->file[i].name, 255);
            LNA.write((char*)&LNA_struct->file[i].load_part_len_name, sizeof(LNA_struct->file[i].len_name));
            LNA.write(LNA_struct->file[i].load_part_name, 512);
        }
        LNA.close();
    }
    free(LNA_struct);
}

void ODownloadThread::receiveCheckedFiles(QStringList checkedFileList)
{
    this->checkedFileList = checkedFileList;
    this->fileListReadable = true;
}

void ODownloadThread::rcvStatusCodeAndMessageSlot(quint16 statusCode, unsigned short totalFileNum, QString statusMessage, bool error, QString errorMessage)
{
    this->statusMessage = statusMessage;
    this->statusCode = statusCode;
    this->totalFileNum = totalFileNum;
    conditionMutex.lock();
    emit(oDownloadStatusMessage(statusMessage));
    if(error == true){
        status = ERROR;
        this->errorMessage = errorMessage;
    }
    else{
        switch (this->statusCode) {
        case 0x0001:
            status = WAIT_LNL_WRQ;
            break;
        case 0x0002:
            status = WAIT_FILE;
            break;
        case 0x0003:
            status = END;
            emit(oDownloadStatusMessage(QString("设备%1下载完成.").arg(device->getName())));
            emit(threadFinish(true, "用户定义下载完成"));
            break;
        default:
            status = ERROR;
            errorMessage = QString(tr("状态码未定义"));
            break;
        }
    }
    statusFileRcved = true;
    statusFileRcvedConditon.wakeOne();
    conditionMutex.unlock();
}


