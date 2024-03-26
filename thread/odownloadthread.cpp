#include "odownloadthread.h"
#include "globalDefine.h"


void ODownloadThread::run()
{
    protocalFileSocket = std::make_shared<QUdpSocket>();
    if(protocalFileSocket->bind(PROTOCAL_FILE_PORT) == false){
        qDebug() << QString("端口号%1被占用").arg(PROTOCAL_FILE_PORT);
        return;
    }
    unsigned short tries = 0;
    const unsigned short DLP_retry = 2;
    while(status != END){
        switch(status){
        case INITIALIZATION:
            while(!Tftp::get(protocalFileSocket.get(), dir.dirName(), QString("%1.LNO").arg(device->getName()), &errorMessage, QHostAddress(device->getHostAddress()), TFTP_SERVER_PORT) &&
                  ++tries < DLP_retry + 1){
                emit(oDownloadStatusMessage(errorMessage));
            }
            if(tries >= DLP_retry + 1){
                emit(oDownloadStatusMessage("超过DLP重传次数"));
                status = ERROR;
                break;
            }
            status = LIST_TRANSFER;
            break;
        case LIST_TRANSFER:
           {
             std::unique_lock<std::mutex> locker(mutex);
//             variable.wait(locker, [this]{return initToListTransfer;
//             });
             while (initToListTransfer == false) {
                variable.wait(locker);
             };
             initToListTransfer = false;
             qDebug() << "操作码为" << LNS.op_stat_code;
             if(LNS.op_stat_code != 0x0001){
                 break;
             }
             if(tftpRequest->mutex.tryLock(13 * 1000) == false){
                 status = ERROR;
                 errorMessage = QString("等待LNL写请求超时");
                 break;
             }
             QByteArray request = tftpRequest->getRequest();
             quint16 port = tftpRequest->getPort();
             QString fileName = request.mid(2).split('\0').at(0);
             tries = 0;
             while(!Tftp::handlePut(protocalFileSocket.get(), dir.dirName(), QString("%1.LNL").arg(device->getName()), &errorMessage, QHostAddress(device->getHostAddress()), port, request) &&
                   ++tries < DLP_retry + 1){
                 emit(oDownloadStatusMessage(errorMessage));
             }
             if(tries >= DLP_retry + 1){
                 emit(oDownloadStatusMessage("超过DLP重传次数"));
                 status = ERROR;
                 break;
             }
             emit(oDownloadStatusMessage("LNL文件接收完成"));
//             qDebug() << device->getName();
//             qDebug() << dir.dirName();
//             qDebug() << fileName;
             QFile LNL(QString("%1/%2.LNL").arg(dir.dirName(), device->getName()));
             if(LNL.open(QIODevice::ReadOnly) == false){
                 status = ERROR;
                 errorMessage = QString("LNL文件打开失败");
                 break;
             }else {
                 File_LNL *struct_LNL = parseLNL(&LNL);
                 if(struct_LNL->file_num == 0){
                     free(&LNL);
                     free(struct_LNL);
                     status = ERROR;
                     errorMessage = QString("没有文件可供用户自定义下载");
                     break;
                 }
                 for(int i = 0; i < struct_LNL->file_num; i++){
                     QPair<QString, QString> pair;
                     pair.first = struct_LNL->file[i].file_name;
                     pair.second = struct_LNL->file[i].file_stat_des;
                     fileList->push_back(pair);
                 }
                 free(struct_LNL);
                 emit(sendFileList(fileList));
                 emit(oDownloadStatusMessage(QString("LNL文件解析成功")));
             }
             //等待用户选择要下载的文件
             unsigned int waitTimes = 0;
             while (!mainThreadExitedOrNot && !fileListReadable && waitTimes < 1000 ) {
                 QThread::msleep(200);
                 waitTimes++;
             }
             qDebug() << "OdownloadThread 93 fileListReadabl:"<<fileListReadable;
             qDebug() << "OdownloadThread 93 fileListReadabl:"<< mainThreadExitedOrNot;
             if(!fileListReadable){
                 status = ERROR;
                 errorMessage = QString("选择时间过长，请重新操作");
                 break;
             }
             if(mainThreadExitedOrNot){
                 status = ERROR;
                 errorMessage = QString("主线程已经退出");
                 break;
             }
             makeLNA();
             emit(oDownloadStatusMessage(QString("LNA文件生成成功")));
             //发送LNA文件写请求
             if(!Tftp::put(protocalFileSocket.get(), dir.dirName(), QString("%1.LNA").arg(device->getName()), &errorMessage, QHostAddress(device->getHostAddress()), TFTP_SERVER_PORT)){
                 status = ERROR;
                 break;
             }
             status = TRANSFER;
             break;
          }

        case TRANSFER:
        {
            if(tftpRequest->mutex.tryLock(13 * 1000) == false){
                status = ERROR;
                errorMessage = QString("等待数据文件读请求超时");
                break;
            }//设置等待数据读取请求的时间
            QByteArray writeRequest = tftpRequest->getRequest();
            quint16 port = tftpRequest->getPort();
            QString fileName = writeRequest.mid(2).split('\0').at(0);

//            qDebug() << writeRequest;
//            qDebug() << fileName;
//            qDebug() << port;
            while (checkedFileList.contains(fileName) && downloadFilesCount < checkedFileList.size()) {
                if(!Tftp::handlePut(protocalFileSocket.get(), dir.dirName(), fileName, &errorMessage, QHostAddress(device->getHostAddress()), port, writeRequest)){
                     status = ERROR;
                     errorMessage = QString("");
                     break;
                }
                downloadFilesCount++;
                emit(oDownloadStatusMessage(QString("文件%1下载完成").arg(fileName)));
                writeRequest = tftpRequest->getRequest();
                fileName = writeRequest.mid(2).split('\0').at(0);
                //TODO 进度条
            }
            status = END;
            break;
        }
        case ERROR:
            emit(oDownloadStatusMessage("用户定义下载操作异常结束"));
            status = END;
            break;
        case END:
            break;
        }
    }
//    this->tftpClient = new QUdpSocket();
//    this->tftpServer = new QUdpSocket();
//    this->tftpClient->connectToHost(device->getHostAddress(), 69);
//    QByteArray request;
//    quint16 port;
//    QString fileName;
//    QFile *LNL = nullptr;
//    unsigned int waitUserTimes = 0;
//    while(status != END){
//        waitTimes = 0;
//        switch (status) {
//        case SEND_LNO_RRQ:
//            if(mainThreadExitedOrNot || !Tftp::receiveFile(tftpClient, QString("%1/%2.LNO").arg(dir.dirName(), device->getName()), &errorMessage, &mainThreadExitedOrNot, Tftp::RRQ)){
//                status = ERROR;
//                break;
//            }
//            emit oDownloadStatusMessage(tr("LNO接收完成"));
//            if(waitStatusFileRcved(errorMessage, max_retrans_times * max_find_response_time_ms) == false){
//                status = ERROR;
//                break;
//            }
//            break;
//        case WAIT_LNL_WRQ:
//            request = tftpRequest->getRequest(&mainThreadExitedOrNot);
//            if(request.size() == 0){
//                status = ERROR;
//                if(mainThreadExitedOrNot){
//                    errorMessage = QString("主进程已退出");
//                }
//                else{
//                    errorMessage = QString("等待LNL写请求超时");
//                }
//                break;
//            }
//            port = tftpRequest->getPort();
//            fileName = request.mid(2).split('\0').at(0);
//            tftpServer->disconnectFromHost();
//            tftpServer->connectToHost(device->getHostAddress(), port);
//            tftpRequest->lockMutex();
//            if(!Tftp::receiveFile(tftpServer, QString("%1/%2").arg(dir.dirName(), fileName), &errorMessage, &mainThreadExitedOrNot, Tftp::WRQ)){
//                status = ERROR;
//                break;
//            }
//            emit(oDownloadStatusMessage(QString(tr("LNL文件接收成功"))));
//            LNL = new QFile(QString("%1/%2").arg(dir.dirName(), fileName));
//            if(!LNL->open(QIODevice::ReadOnly)){
//                status = ERROR;
//                errorMessage = QString("LNL文件打开失败");
//                break;
//            }
//            else{
//                File_LNL *LNL_struct = parseLNL(LNL->readAll());
//                fileList = new QList<QPair<QString, QString>>();
//                if(LNL_struct->file_num == 0){
//                    free(LNL_struct);
//                    status = ERROR;
//                    errorMessage = QString(tr("没有文件可供用户定义下载"));
//                    break;
//                }
//                for(int i = 0; i < LNL_struct->file_num; i++){
//                    QPair<QString, QString> pair;
//                    pair.first = QString(LNL_struct->file[i].file_name);
//                    pair.second = QString(LNL_struct->file[i].file_stat_des);
//                    fileList->push_back(pair);
//                }

//                free(LNL_struct);
//                emit sendFileList(fileList);
//                emit oDownloadStatusMessage(QString(tr("LNL文件解析成功")));
//                status = SEND_LNA_WRQ;
//            }
//            break;
//        case SEND_LNA_WRQ:
//            while(!mainThreadExitedOrNot && !fileListReadable && waitUserTimes < 1000){
//                QThread::msleep(200);
//                waitUserTimes++;
//            }
//            if(mainThreadExitedOrNot){
//                errorMessage = QString("主线程已退出");
//                status = ERROR;
//                break;
//            }
//            if(!fileListReadable){
//                status = ERROR;
//                errorMessage = QString("等待用户选择超时");
//                break;
//            }
//            emit oDownloadStatusMessage(QString(tr("获取下载文件列表成功")));
//            makeLNA();
//            emit oDownloadStatusMessage(QString(tr("LNA文件构造成功")));
//            if(!Tftp::sendFile(tftpClient, QString("%1/%2.LNA").arg(dir.dirName(), device->getName()), &errorMessage, &mainThreadExitedOrNot, Tftp::WRQ)){
//                status = ERROR;
//                break;
//            }
//            emit oDownloadStatusMessage(QString(tr("LNA文件发送成功")));
//            if(waitStatusFileRcved(errorMessage, max_retrans_times * max_find_response_time_ms) == false){
//                status = ERROR;
//                break;
//            }
//        case WAIT_FILE:{
//            QByteArray request = tftpRequest->getRequest(&mainThreadExitedOrNot);
//            if(request.size() == 0){
//                if(mainThreadExitedOrNot){
//                    errorMessage = QString("主进程已退出");
//                }
//                else{
//                    errorMessage = QString("等待设备端TFTP写请求超时");
//                }
//                status = ERROR;
//                break;
//            }
//            quint16 port = tftpRequest->getPort();
//            QString fileName = request.mid(2).split('\0').at(0);
//            tftpServer->disconnectFromHost();
//            tftpServer->connectToHost(device->getHostAddress(), port);
//            tftpRequest->lockMutex();
//            if(!Tftp::receiveFile(tftpServer, QString("%1/%2").arg(dir.dirName(), fileName), &errorMessage, &mainThreadExitedOrNot,Tftp::WRQ)){
//                status = ERROR;
//                break;
//            }
//            emit oDownloadStatusMessage(QString("设备%1: 文件%2下载完成(%3/%4)").
//                                        arg(device->getName(),
//                                            fileName,
//                                            QString::number(transmitFileNum),
//                                            QString::number(totalFileNum)));
//            emit oDownloadRate(100 * transmitFileNum++ / totalFileNum, true);
//            if(waitStatusFileRcved(errorMessage, max_retrans_times * max_find_response_time_ms) == false){
//                status = ERROR;
//                break;
//            }
//            break;
//        }
//        case ERROR:
//            emit oDownloadStatusMessage(QString("设备%1: %2").arg(device->getName(), errorMessage));
//            emit oDownloadStatusMessage(QString("用户定义下载异常结束"));
//            emit oDownloadRate(0, false);
//            emit threadFinish(false, errorMessage);
//            status = END;
//            break;
//        case END:
//            break;
//        default:
//            break;
//        }
//    }
}


File_LNL *ODownloadThread::parseLNL(QFile* file)
{
      File_LNL* struct_LNL = new File_LNL();
      memset(struct_LNL, 0, sizeof(File_LNL));
      QDataStream in(file);

#ifdef BIG_ENDIAN
      in.setByteOrder(QDataStream::BigEndian);
#else
      in.setByteOrder(QDataStream::LittleEndian);
#endif

      in >> struct_LNL->file_len;
      in.readRawData(struct_LNL->Pro_ver, 2);
      in >> struct_LNL->file_num;
      if(struct_LNL->file_num == 0){
          emit(oDownloadStatusMessage(QString("LNL文件解析失败，总数为0")));
      }
      struct_LNL->file = (struct file_info_LNL*)malloc(sizeof(file_info_LNL) * struct_LNL->file_num);
      for(int i = 0; i < struct_LNL->file_num; i++){
          in >> struct_LNL->file[i].file_name_len;
          in.readRawData( struct_LNL->file[i].file_name, 255);
          in >> struct_LNL->file[i].file_stat_des_len;
          in.readRawData( struct_LNL->file[i].file_stat_des, 255);
      }

      file->close();
      return struct_LNL;
}

void ODownloadThread::makeLNA()
{
    File_LNA* struct_LNA = new File_LNA();
    struct_LNA->file_len = 8 + sizeof(file_info_LNA) * checkedFileList.size();
    memcpy(struct_LNA->Pro_ver, Protocol_ver, 2);
//    struct_LNA -> file_len =  TODO
    struct_LNA->file_num = checkedFileList.size();
    struct_LNA->file = ( struct file_info_LNA*)malloc(sizeof (file_info_LNA) * checkedFileList.count());
    for(int i = 0; i < checkedFileList.size(); i++){
        strcpy(struct_LNA->file[i].file_name, checkedFileList.at(i).toUtf8().data());
        struct_LNA -> file[i].file_name_len = checkedFileList.at(i).size();
    }
    QFile LNA(QString("%1/%2.LNA").arg(dir.dirName(), device->getName()));
    if(LNA.open(QIODevice::WriteOnly) == false){
        status = ERROR;
        errorMessage = QString("LNA文件生成错误");
        return;
    }
    QDataStream out(&LNA);
#ifdef BIG_ENDIAN
    out.setByteOrder(QDataStream::BigEndian);
#else
    out.setByteOrder(QDataStream::LittleEndian);
#endif

    out << struct_LNA->file_len;
    out.writeRawData(struct_LNA->Pro_ver, 2);
    out << struct_LNA->file_num;
    for(int i = 0; i < struct_LNA->file_num; i++){
        out << struct_LNA->file[i].file_name_len;
        out.writeRawData(struct_LNA->file[i].file_name, 255);
    }
    LNA.close();
    free(struct_LNA);
}

void ODownloadThread::receiveCheckedFiles(QStringList checkedFileList)
{
    this->checkedFileList = checkedFileList;
    this->fileListReadable = true;
}

void ODownloadThread::rcvStatusCodeAndMessageSlot(quint16 statusCode, unsigned short totalFileNum, QString statusMessage, bool error, QString errorMessage)
{
//    this->statusMessage = statusMessage;
//    this->statusCode = statusCode;
//    this->totalFileNum = totalFileNum;
//    conditionMutex.lock();
//    emit(oDownloadStatusMessage(statusMessage));
//    if(error == true){
//        status = ERROR;
//        this->errorMessage = errorMessage;
//    }
//    else{
//        switch (this->statusCode) {
//        case 0x0001:
//            status = WAIT_LNL_WRQ;
//            break;
//        case 0x0002:
//            status = WAIT_FILE;
//            break;
//        case 0x0003:
//            status = END;
//            emit(oDownloadStatusMessage(QString("设备%1下载完成.").arg(device->getName())));
//            emit(threadFinish(true, "用户定义下载完成"));
//            break;
//        default:
//            status = ERROR;
//            errorMessage = QString(tr("状态码未定义"));
//            break;
//        }
//    }
//    statusFileRcved = true;
//    statusFileRcvedConditon.wakeOne();
//    conditionMutex.unlock();
}



void ODownloadThread::parseStatusFile()
{
//    free(LNS.LNS_files);
    qDebug() << "进入ODownload线程的parseStatusFile";
    QFile fLNS(QString("%1/%2.LNS").arg(dir.dirName()).arg(device->getName()));
    if(fLNS.open(QIODevice::ReadOnly) == false){
        return;
    }
    memset(&LNS, 0, sizeof(File_LNS));
    QDataStream in(&fLNS);
    in.setByteOrder(QDataStream::LittleEndian);

    in >> LNS.file_len;
    in >> LNS.Pro_ver;
    in >> LNS.op_stat_code;
    in >> LNS.stat_des_len;
    in.readRawData(LNS.stat_des, LNS.stat_des_len);
    in >> LNS.counter;
    in >> LNS.excep_timer;
    in >> LNS.estim_timer;
    in.readRawData(LNS.down_list_ratio, 3);
    in >> LNS.file_num;
    if(LNS.file_num > 0) LNS.LNS_files = (struct file_info_LNS*)malloc(sizeof(file_info_LNS) * LNS.file_num);
    for(int i= 0; i < LNS.file_num; i++){
        in >> LNS.LNS_files[i].file_name_len;
        in.readRawData(LNS.LNS_files[i].file_name, 255);
        in >> LNS.LNS_files[i].file_stat;
        in >> LNS.LNS_files[i].file_stat_des_len;
        in.readRawData(LNS.LNS_files[i].file_stat_des, 255);
    }
    fLNS.close();
    emit(parseStatusFileFinished(LNS));

    std::unique_lock<std::mutex> locker(mutex);
    initToListTransfer = true;
    variable.notify_one();
    qDebug() << "完成ODownload线程的parseStatusFile";
    return;
//    if(LNS.op_stat_code == 0x0001){
//        locker.unlock();
//        variable.notify_one();
//    }else {
//        locker.unlock();
//    }
}


