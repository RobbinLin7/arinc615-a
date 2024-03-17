#include "mdownloadthread.h"

void MDownloadThread::run()
{
//    this->tftpClient = new QUdpSocket();
//    this->tftpServer = new QUdpSocket();
//    this->tftpClient->connectToHost(device->getHostAddress(), 69);
//    int totalTransNum = this->fileList.size();
//    int transNum = 0;
//    while(status != END){
//        waitTimes = 0;
//        switch (status) {
//        case SEND_LND_RRQ:
//        {
//            if(!Tftp::receiveFile(tftpClient, QString("%1/%2.LND").arg(dir.dirName(), device->getName()), &errorMessage, &mainThreadExitedOrNot, Tftp::RRQ)){
//                status = ERROR;
//                break;
//            }
//            if(waitStatusFileRcved(errorMessage, max_retrans_times * max_find_response_time_ms) == false){
//                status = ERROR;
//                break;
//            }
//            break;
//        }
//        case SEND_LNR_WRQ:
//        {
//            makeLNR();
//            if(!Tftp::sendFile(tftpClient, QString("%1/%2.LNR").arg(dir.dirName(), device->getName()), &errorMessage, &mainThreadExitedOrNot, Tftp::WRQ)){
//                status = ERROR;
//                break;
//            }
//            if(waitStatusFileRcved(errorMessage, max_retrans_times * max_find_response_time_ms) == false){
//                status = ERROR;
//                break;
//            }
//            break;
//        }
//        case WAIT_FILE_WRQ:{
//            //可能是第一个数据文件，也可能是最后一个LNS文件
//            QByteArray request = tftpRequest->getRequest(&mainThreadExitedOrNot);
//            if(request.size() == 0){
//                errorMessage = QString("等待设备端TFTP请求超时");
//                status = ERROR;
//                break;
//            }
//            quint16 port = tftpRequest->getPort();
//            QString fileName = request.mid(2).split('\0').at(0);
//            tftpServer->disconnectFromHost();
//            tftpServer->connectToHost(device->getHostAddress(), port);
//            tftpRequest->lockMutex();
//            if(!Tftp::receiveFile(tftpServer, QString("%1/%2").arg(dir.dirName(), fileName), &errorMessage, &mainThreadExitedOrNot, Tftp::WRQ)){
//                status = ERROR;
//                break;
//            }
//            emit mDownloadStatusMessage(QString("设备%1: 文件%2下载完成.(%3/%4)")
//                                        .arg(device->getName())
//                                        .arg(fileName)
//                                        .arg(++transNum)
//                                        .arg(totalTransNum));
//            emit mDownloadRate(transNum * 100 / totalTransNum, true);
//            if(waitStatusFileRcved(errorMessage, max_retrans_times * max_find_response_time_ms) == false){
//                status = ERROR;
//                break;
//            }
//            break;
//        }
//        case ERROR:
//            emit mDownloadStatusMessage(QString("设备%1: %2").arg(device->getName(), errorMessage));
//            emit mDownloadStatusMessage(QString("介质定义下载异常结束"));
//            emit mDownloadRate(0, false);
//            status = END;
//            break;
//        case END:
//            break;
//        default:
//            break;
//        }
//    }
//    emit threadFinish(true, QString(("介质定义下载结束")));
}

void MDownloadThread::makeLNR()
{
    File_LNR LNR_struct;
    memset(&LNR_struct, 0, sizeof(File_LNR));
    LNR_struct.file_len = sizeof(LNR_struct.file_len) + sizeof(LNR_struct.Pro_ver) + sizeof(LNR_struct.file_num);
    memcpy(LNR_struct.Pro_ver, Protocol_ver, 2);
    LNR_struct.file_num = fileList.size();

    LNR_struct.hfile = (Hfile_info*)malloc(sizeof(Hfile_info) * fileList.size());
    memset(LNR_struct.hfile, 0, sizeof(Hfile_info) * fileList.size());
    for(int i = 0; i < LNR_struct.file_num; i++){
        LNR_struct.hfile[i].len_name = fileList.at(i).size();
        strcpy(LNR_struct.hfile[i].name, fileList.at(i).toUtf8().data());
        LNR_struct.file_len += sizeof (LNR_struct.hfile[i].name) + sizeof (LNR_struct.hfile[i].len_name);
    }
//    qToBigEndian(LNR_struct.Pro_ver, &LNR_struct.Pro_ver);
//    qToBigEndian(LNR_struct.file_num, &LNR_struct.file_num);
//    qToBigEndian(LNR_struct.file_len, &LNR_struct.file_len);
    QFile LNR(QString("%1/%2.LNR").arg(dir.dirName(), device->getName()));
    if(LNR.open(QIODevice::WriteOnly)){
        LNR.write((char*)&LNR_struct.file_len, sizeof(LNR_struct.file_len));
        LNR.write(LNR_struct.Pro_ver, sizeof(LNR_struct.Pro_ver));
        LNR.write((char*)&LNR_struct.file_num, sizeof(LNR_struct.file_num));
        LNR.write((char*)&LNR_struct.user_def_len, sizeof(LNR_struct.user_def_len));
        LNR.write(LNR_struct.user_def, 255);
        for(int i = 0; i < fileList.size(); i++){
            LNR.write((char*)&LNR_struct.hfile[i].len_name,
                      sizeof(LNR_struct.hfile[i].len_name));
            LNR.write(LNR_struct.hfile[i].name, 255);
            LNR.write((char*)&LNR_struct.hfile[i].load_part_len_name,
                      sizeof(LNR_struct.hfile[i].load_part_len_name));
            LNR.write(LNR_struct.hfile[i].load_part_name, 512);
        }
        LNR.close();
    }
    free(LNR_struct.hfile);
}

File_LNS *MDownloadThread::parseLNS(const QByteArray data)
{
    File_LNS *LNS = (File_LNS*)malloc(sizeof(File_LNS));
    LNS->file_len = data[0] + ((quint32)data[1] << 8) + ((uint32)data[2] << 16) + ((uint32)data[3] << 24);
    LNS->Pro_ver = data[4] + ((quint16)data[5] << 8);
    LNS->op_stat_code = data[6] + ((quint16)data[7] << 8);
    LNS->stat_des_len = data[8];
    strcpy(LNS->stat_des, QString::fromStdString(data.mid(9, LNS->stat_des_len).toStdString()).toUtf8().data());
    unsigned int pos = 9 + sizeof(LNS->stat_des);
    LNS->counter = ((quint16)data[pos] << 8) + data[pos + 1];
    pos += 2;
    LNS->excep_timer = ((quint16)data[pos] << 8) + data[pos + 1];
    pos += 2;
    LNS->estim_timer = ((quint16)data[pos] << 8) + data[pos + 1];
    pos += 2;
    memcpy(LNS->down_list_ratio, data.mid(pos, 3).data(), 3);
    pos += 3;
    LNS->file_num = ((quint16)data[pos] << 8) + data[pos + 1];
    LNS->LNS_files = (file_info_LNS *)malloc(sizeof(file_info_LNS) * LNS->file_num);
    for(int i = 0; i < LNS->file_num; i++){
        LNS->LNS_files[i].file_name_len = (unsigned char)data[pos++];
        strcpy(LNS->LNS_files[i].file_name, data.mid(pos, LNS->LNS_files[i].file_name_len).data());
        pos += sizeof(LNS->LNS_files[i].file_name);
        LNS->LNS_files[i].file_stat = ((quint16)data[pos] << 8) + data[pos + 1];
        pos += 2;
        LNS->LNS_files[i].file_stat_des_len = (unsigned char)data[pos++];
        strcpy(LNS->LNS_files[i].file_stat_des, data.mid(pos, LNS->LNS_files[i].file_stat_des_len).data());
        pos += sizeof(LNS->LNS_files[i].file_stat_des);
    }
    return LNS;
}

File_LNS *MDownloadThread::parseLNS()
{
    File_LNS *LNS_struct = (File_LNS*)malloc(sizeof(File_LNS));
    QFile LNS(QString("%1/%2.LNS").arg(dir.dirName(), device->getName()));
    if(!LNS.open(QIODevice::ReadOnly)){
        free(LNS_struct);
        status = ERROR;
        errorMessage = QString("LNS文件打开失败");
        return nullptr;
    }
    else{
        QDataStream in(&LNS);
        in.setByteOrder(QDataStream::LittleEndian);
        in >> LNS_struct->file_len;
        qDebug() << (quint32)LNS_struct->file_len;
        in >> LNS_struct->Pro_ver;
        qDebug() << LNS_struct->Pro_ver;
        in >> LNS_struct->op_stat_code;
        qDebug() << LNS_struct->op_stat_code;
        in >> LNS_struct->stat_des_len;
        in.readRawData(LNS_struct->stat_des, LNS_struct->stat_des_len);
        in >> LNS_struct->counter;
        in >> LNS_struct->excep_timer;
        in >> LNS_struct->estim_timer;
        in.readRawData(LNS_struct->down_list_ratio, 3);
        in >> LNS_struct->file_num;
        LNS_struct->LNS_files = (file_info_LNS *)malloc(sizeof(file_info_LNS) * LNS_struct->file_num);
        for(int i = 0; i < LNS_struct->file_num; i++){
            in >> LNS_struct->LNS_files[i].file_name_len;
            in.readRawData(LNS_struct->LNS_files[i].file_name, LNS_struct->LNS_files[i].file_name_len);
            in >> LNS_struct->LNS_files[i].file_stat;
            in >> LNS_struct->LNS_files[i].file_stat_des_len;
            in.readRawData(LNS_struct->LNS_files[i].file_stat_des, LNS_struct->LNS_files[i].file_stat_des_len);
        }
        LNS.close();
    }
    return LNS_struct;
}


void MDownloadThread::rcvStatusCodeAndMessageSlot(quint16 statusCode, unsigned short totalFileNum, QString statusMessage, bool error, QString errorMessage)
{
    this->statusMessage = statusMessage;
    this->statusCode = statusCode;
    this->totalFileNum = totalFileNum;
    emit(mDownloadStatusMessage(statusMessage));
    conditionMutex.lock();
    if(error == true){
        status = ERROR;
        this->errorMessage = errorMessage;
    }
    else{
        switch (this->statusCode) {
        case 0x0001:
            status = SEND_LNR_WRQ;
            break;
        case 0x0002:
            status = WAIT_FILE_WRQ;
            break;
        case 0x0003:
            status = END;
            emit(mDownloadStatusMessage(QString("设备%1下载完成.").arg(device->getName())));
            emit(threadFinish(true, "介质定义下载完成"));
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

void MDownloadThread::parseStatusFile()
{

}
