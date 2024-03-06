#include "uploadthread.h"

void UploadThread::run()
{
    protocalFileSocket = std::make_shared<QUdpSocket>();
    if(protocalFileSocket->bind(PROTOCAL_FILE_PORT) == false){
        qDebug() << QString("端口号%1被占用").arg(PROTOCAL_FILE_PORT);
        return;
    }
    QByteArray request;
    quint16 port;
    QString fileName;
    QFile LUR(QString("%1/%2.LUR").arg(dir.dirName(), device->getName()));
    QFile LUH(QString("%1/%2.LUH").arg(dir.dirName(), device->getName()));
    bool flag = false;
    const unsigned short DLP_retry = 2;
    unsigned short tries = 0;
    QString errorMessage;
    while(status != END){
        //waitTimes = 0;
        switch (status) {
        case SEND_LUI_RRQ:
            while(!Tftp::get(protocalFileSocket.get(), dir.dirName(), QString("%1.LUI").arg(device->getName()), &errorMessage, QHostAddress(device->getHostAddress()), TFTP_SERVER_PORT) &&
                  ++tries < DLP_retry + 1){}
            if(tries >= DLP_retry + 1){
                status = ERROR;
                break;
            }
            emit(uploadStatusMessage(QString("LUI发送完成")));
            status = SEND_LUR_WRQ;
            break;
        case SEND_LUR_WRQ:
            makeLUR();
            if(!LUR.exists()){
                errorMessage = QString("LUR文件创建失败");
                status = ERROR;
                break;
            }
            if(!Tftp::put(protocalFileSocket.get(), dir.dirName(), QString("%1.LUR").arg(device->getName()), &errorMessage, QHostAddress(device->getHostAddress()), TFTP_SERVER_PORT)){
                status = ERROR;
                break;
            }
//            if(!Tftp::sendFile(tftpClient, QString("%1/%2.LUR").arg(dir.dirName(), device->getName()), &errorMessage, &mainThreadExitedOrNot, Tftp::WRQ)){
//                status = ERROR;
//                break;
//            }
//            emit(uploadStatusMessage("LUR发送完成"));
            status = WAIT_FILE_RRQ;
            break;
        case WAIT_FILE_RRQ:
            if(tftpRequest->mutex.tryLock(13 * 1000) == false){
                status = ERROR;
                errorMessage = QString("等待数据文件读请求超时");
                break;
            }
            request = tftpRequest->getRequest();
            port = tftpRequest->getPort();
//            tftpRequest->mutex.unlock();
//            if(request.size() == 0){
//                status = ERROR;
//                errorMessage = QString("等待数据文件读请求超时");
//                break;
//            }
            fileName = request.mid(2).split('\0').at(0);
            qDebug() << "fileName = " << fileName;
            for(int i = 0; i < fileList.size(); ++i){
                if(fileList.at(i).contains(fileName)){
                    if(Tftp::handleGet(protocalFileSocket.get(), fileList.at(i).left(fileList.at(i).lastIndexOf('/')), fileName, &errorMessage, QHostAddress(device->getHostAddress()), port, request) == false){
                        qDebug() << "upload file " << fileName << "error";
                    }
                    else {
                        ++fileSentCnt;
                        emit(uploadStatusMessage(QString("文件%1上传完成").arg(fileName)));
                        emit(uploadRate(fileSentCnt * 100 / fileList.size(), true));
                    }
                    break;
                }
            }
            if(fileSentCnt == fileList.size()){
                status = END;
            }
            break;
//        case WAIT_LUH_RRQ:
//            request = tftpRequest->getRequest(&mainThreadExitedOrNot);
//            if(request.size() == 0){
//                status = ERROR;
//                errorMessage = mainThreadExitedOrNot ? QString(tr("主线程已退出")) : QString("等待LUH文件读请求超时");
//                break;
//            }
//            port = tftpRequest->getPort();
//            fileName = request.mid(2).split('\0').at(0);
//            tftpRequest->lockMutex();
//            tftpServer->disconnectFromHost();
//            tftpServer->connectToHost(device->getHostAddress(), port);
//            if(fileName.endsWith(".LUH")){
//                makeLUH();
//                if(!LUH.exists()){
//                    status = ERROR;
//                    errorMessage = QString("LUH文件创建失败");
//                    break;
//                }
//                if(!Tftp::sendFile(tftpServer, QString("%1\\%2.LUH").arg(dir.dirName(), device->getName()), &errorMessage, &mainThreadExitedOrNot, Tftp::RRQ)){
//                    status = ERROR;
//                    break;
//                }
//            }
//            else{
//                status = ERROR;
//                errorMessage = QString("未知文件请求错误");
//                break;
//            }
//            emit(uploadStatusMessage("LUH发送完成"));
//            status = WAIT_FILE_RRQ;
//            break;
//        case WAIT_FILE_RRQ:
//            request = tftpRequest->getRequest(&mainThreadExitedOrNot);
//            if(request.size() == 0){
//                status = ERROR;
//                errorMessage = mainThreadExitedOrNot ? QString(tr("主线程已退出")) : QString("等待上传文件读请求超时");
//                break;
//            }
//            port = tftpRequest->getPort();
//            fileName = request.mid(2).split('\0').at(0);
//            tftpRequest->lockMutex();
//            tftpServer->disconnectFromHost();
//            tftpServer->connectToHost(device->getHostAddress(), port);
//            int ii;
//            for(int i = 0; i < fileList.size(); i++){
//                if(fileList.at(i).contains(fileName)){
//                    if(!Tftp::sendFile(tftpServer, fileList.at(i), &errorMessage, &mainThreadExitedOrNot, Tftp::RRQ)){
//                        status = ERROR;
//                        flag = true;
//                    }
//                    if(files_sent[fileName] == false){
//                        fileSentCnt++;
//                        files_sent[fileName] = true;
//                    }
//                    ii = i;
//                    break;
//                }
//            }
//            if(flag) break;
//            emit(uploadRate(fileSentCnt * 100 / fileList.size(), true));
//            emit(uploadStatusMessage(QString("设备%1: 文件%2上传完成.(%3/%4)")
//                                        .arg(device->getName())
//                                        .arg(fileList.at(ii))
//                                        .arg(fileSentCnt)
//                                        .arg(fileList.size())));
//            QThread::msleep(200);
//            break;
        case ERROR:
            emit(uploadStatusMessage(errorMessage));
            //emit(uploadStatusMessage(QString(tr("上传操作异常结束"))));
            emit(uploadResult(false));
            emit(uploadRate(0, false));
            status = END;
            break;
        case END:
            emit(uploadStatusMessage(QString("上传操作结束")));
            break;
        default:
            break;
        }
    }
    emit(threadFinish(UPLOAD_OP_CODE, QString(tr("上传操作结束"))));
    emit(uploadStatusMessage(QString("上传操作结束")));
    protocalFileSocket->close();
    qDebug() << "mainThreadExitedOrNot状态" << mainThreadExitedOrNot;
    if(subOfAuto) tftpRequest = nullptr;
}

void UploadThread::makeLUR(){
    QFile LUR(QString("%1/%2.LUR").arg(dir.dirName(), device->getName()));
    //LUR.open(QIODevice::WriteOnly);
    File_LUR LUR_struct;
    memcpy(LUR_struct.Pro_ver, Protocol_ver, 2);
    LUR_struct.Header_num = fileList.size();
    LUR_struct.file_len = sizeof(LUR_struct.Pro_ver) + sizeof(LUR_struct.Header_num) + sizeof(LUR_struct.file_len);
    LUR_struct.Hfile = (HFILE_INFO*) malloc(LUR_struct.Header_num * sizeof(HFILE_INFO));
    for(int i = 0; i < LUR_struct.Header_num; i++){
        QFileInfo fileInfo(fileList.at(i));
        LUR_struct.Hfile[i].fileLen = fileInfo.size();
        memset(LUR_struct.Hfile[i].name, 0 ,sizeof (LUR_struct.Hfile[i].name));
        strcpy(LUR_struct.Hfile[i].name, fileList.at(i).mid(fileList.at(i).lastIndexOf('/') + 1).toStdString().c_str());
        LUR_struct.Hfile[i].len_name = strlen(LUR_struct.Hfile[i].name);
        //LUR_struct.file_len += sizeof(LUR_struct.Hfile[i].len_name) + LUR_struct.Hfile[i].len_name;
        memset(LUR_struct.Hfile[i].load_part_name, 0, sizeof(LUR_struct.Hfile[i].load_part_name));
        strcpy(LUR_struct.Hfile[i].load_part_name, dir.absolutePath().toStdString().c_str());
        LUR_struct.Hfile[i].load_part_len_name = strlen(LUR_struct.Hfile[i].load_part_name);
        //LUR_struct.file_len += sizeof(LUR_struct.Hfile[i].load_part_len_name) + LUR_struct.Hfile[i].load_part_len_name;
        LUR_struct.file_len += sizeof(LUR_struct.Hfile[i].load_part_len_name) + sizeof(LUR_struct.Hfile[i].len_name);
        LUR_struct.file_len += strlen(LUR_struct.Hfile[i].load_part_name) + 1;
        LUR_struct.file_len += strlen(LUR_struct.Hfile[i].name) + 1;
    }
    if(LUR.open(QIODevice::WriteOnly)){
        QDataStream os(&LUR);
#ifdef BIG_ENDIAN
        os.setByteOrder(QDataStream::BigEndian);
#else
        os.setByteOrder(QDataStream::LittleEndian);
#endif
        os << LUR_struct.file_len;
        os.writeRawData(LUR_struct.Pro_ver, sizeof(LUR_struct.Pro_ver));
        os << LUR_struct.Header_num;
        for(int i = 0; i < LUR_struct.Header_num; ++i){
            os << LUR_struct.Hfile[i].fileLen;
            os << LUR_struct.Hfile[i].len_name;
            os.writeRawData(LUR_struct.Hfile[i].name, LUR_struct.Hfile[i].len_name + 1);
            os << LUR_struct.Hfile[i].load_part_len_name;
            os.writeRawData(LUR_struct.Hfile[i].load_part_name, LUR_struct.Hfile[i].load_part_len_name + 1);
        }
        LUR.close();
    }
    free(LUR_struct.Hfile);
//    LUR.write((char *)&LUR_struct.file_len, sizeof(LUR_struct.file_len));
//    LUR.write(LUR_struct.Pro_ver, sizeof(LUR_struct.Pro_ver));
//    LUR.write((char *)&LUR_struct.Header_num, sizeof(LUR_struct.Header_num));
//    for(int i = 0; i < LUR_struct.Header_num; i++){
//        LUR.write((char *)&LUR_struct.Hfile[i].len_name, sizeof(LUR_struct.Hfile[i].len_name));
//        LUR.write(LUR_struct.Hfile[i].name, LUR_struct.Hfile[i].len_name);
//        LUR.write((char *)&LUR_struct.Hfile[i].load_part_len_name, sizeof(LUR_struct.Hfile[i].load_part_len_name));
//        LUR.write(LUR_struct.Hfile[i].load_part_name, LUR_struct.Hfile[i].load_part_len_name);
//    }
//    LUR.close();
//    free(LUR_struct.Hfile);
}

void UploadThread::makeLUH()
{
    QFile LUH(QString("%1/%2.LUH").arg(dir.dirName(), device->getName()));
    LUH.open(QIODevice::ReadWrite | QIODevice::Truncate);
    File_LUH LUH_struct;
    int i = 0;
    memset(&LUH_struct, 0, sizeof(File_LUH));
    LUH_struct.DataFile_Num = fileList.size();
    LUH_struct.Ptr_DataFile_Num = fileList.size();
    LUH_struct.datafile = (DataFile*)malloc(sizeof (DataFile) * LUH_struct.DataFile_Num);
    LUH_struct.file_len = sizeof(LUH_struct.file_len) + sizeof (LUH_struct.LoadFile_Ver);
    LUH_struct.file_len += sizeof(LUH_struct.Part_Flag) + sizeof(LUH_struct.Ptr_PN_Len);
    LUH_struct.file_len += sizeof(LUH_struct.Ptr_TargetID_Num) + sizeof(LUH_struct.Ptr_DataFile_Num);
    LUH_struct.file_len += sizeof(LUH_struct.Ptr_SupFile_Num) + sizeof(LUH_struct.Ptr_UserDef_Data);
    LUH_struct.file_len += sizeof(LUH_struct.Ptr_LoadTypeDes_Len) + sizeof(LUH_struct.Ptr_TargetIDPosition_Num);
    LUH_struct.file_len += sizeof(LUH_struct.Ptr_LoadCheckVal_Len) + sizeof(LUH_struct.DataFile_Num);
    LUH_struct.file_len += sizeof(LUH_struct.LoadCheckVal_Type) + sizeof(LUH_struct.HFile_CRC);
    memset(LUH_struct.datafile, 0, LUH_struct.DataFile_Num * sizeof(DataFile));
    qDebug() << LUH_struct.file_len << "---1";
    for(i = 0; i < LUH_struct.DataFile_Num; ++i)
    {
        //结构体内容转换
        LUH_struct.file_len += 789;
        int lastSlash = fileList.at(i).lastIndexOf('/');
        QString path = fileList.at(i).left(lastSlash);
        QString name = fileList.at(i).mid(lastSlash + 1);
        LUH_struct.datafile[i].DataFile_Name_Len = name.size();
        memset(LUH_struct.datafile[i].DataFile_Name, 0, sizeof(LUH_struct.datafile[i].DataFile_Name));
        strcpy(LUH_struct.datafile[i].DataFile_Name, name.toUtf8().data());
        LUH_struct.datafile[i].DataFile_PN_Len = ((path.size() + 1 + 1)/ 2) * 2;
        memset(LUH_struct.datafile[i].DataFile_PN, 0, LUH_struct.datafile[i].DataFile_PN_Len);
        QFile file(fileList.at(i));
        char to_crc[512];
        if(file.open(QIODevice::ReadOnly)){
            LUH_struct.datafile[i].DataFile_Len = file.size();
            unsigned short crc = 0;
            size_t nBytes;
            memset(to_crc, 0, sizeof(to_crc));
//            memcpy(to_crc, file.readAll(), file.size());
//            crc = crc16_table(to_crc, file.size(), 0);
            while((nBytes = file.read(to_crc, 512)) > 0){
                crc = crc16_table(to_crc, nBytes, crc);
            }
            LUH_struct.datafile[i].DataFile_CRC = crc;
            //free(to_crc);
            qDebug()<<"16进制小写输出"<<QString("%1").arg(crc,4,16,QLatin1Char('0'));
            file.close();
        }
        LUH_struct.datafile[i].DataFile_LenInBytes = LUH_struct.datafile[i].DataFile_Len;
        LUH_struct.datafile[i].DataFile_CheckVal_Len = 0;
        memset(LUH_struct.datafile[i].DataFile_CheckVal, 0, 2);

        if(i + 1 == LUH_struct.DataFile_Num)
            LUH_struct.datafile[i].Ptr_DataFile = 0;

    }
    qDebug() << LUH_struct.file_len << "---2";
    LUH.write((char *)&LUH_struct.file_len, sizeof(LUH_struct.file_len));
    LUH.write((char *)&LUH_struct.LoadFile_Ver, sizeof(LUH_struct.LoadFile_Ver));
    LUH.write((char *)&LUH_struct.Part_Flag, sizeof(LUH_struct.Part_Flag));
    LUH.write((char *)&LUH_struct.Ptr_PN_Len, sizeof(LUH_struct.Ptr_PN_Len));
    LUH.write((char *)&LUH_struct.Ptr_TargetID_Num, sizeof(LUH_struct.Ptr_TargetID_Num));
    LUH.write((char *)&LUH_struct.Ptr_DataFile_Num, sizeof(LUH_struct.Ptr_DataFile_Num));
    LUH.write((char *)&LUH_struct.Ptr_SupFile_Num, sizeof(LUH_struct.Ptr_SupFile_Num));
    LUH.write((char *)&LUH_struct.Ptr_UserDef_Data, sizeof(LUH_struct.Ptr_UserDef_Data));
    LUH.write((char *)&LUH_struct.Ptr_LoadTypeDes_Len, sizeof(LUH_struct.Ptr_LoadTypeDes_Len));
    LUH.write((char *)&LUH_struct.Ptr_TargetIDPosition_Num, sizeof(LUH_struct.Ptr_TargetIDPosition_Num));
    LUH.write((char *)&LUH_struct.Ptr_LoadCheckVal_Len, sizeof(LUH_struct.Ptr_LoadCheckVal_Len));
    LUH.write((char *)&LUH_struct.DataFile_Num, sizeof(LUH_struct.DataFile_Num));
    for(i = 0; i < LUH_struct.DataFile_Num; ++i)
    {
        LUH.write((char*)&LUH_struct.datafile[i].Ptr_DataFile, sizeof (LUH_struct.datafile[i].Ptr_DataFile));
        //qToBigEndian(LUH_struct.datafile[i].DataFile_Name_Len, &LUH_struct.datafile[i].DataFile_Name_Len);
        LUH.write((char*)&LUH_struct.datafile[i].DataFile_Name_Len, sizeof (LUH_struct.datafile[i].DataFile_Name_Len));
        LUH.write((char*)&LUH_struct.datafile[i].DataFile_Name, sizeof (LUH_struct.datafile[i].DataFile_Name));
        //qToBigEndian(LUH_struct.datafile[i].DataFile_PN_Len, &LUH_struct.datafile[i].DataFile_PN_Len);
        LUH.write((char*)&LUH_struct.datafile[i].DataFile_PN_Len, sizeof (LUH_struct.datafile[i].DataFile_PN_Len));
        LUH.write((char*)&LUH_struct.datafile[i].DataFile_PN, sizeof (LUH_struct.datafile[i].DataFile_PN));
        //qToBigEndian(LUH_struct.datafile[i].DataFile_Len, &LUH_struct.datafile[i].DataFile_Len);
        LUH.write((char*)&LUH_struct.datafile[i].DataFile_Len, sizeof (LUH_struct.datafile[i].DataFile_Len));
        //qToBigEndian(LUH_struct.datafile[i].DataFile_CRC, &LUH_struct.datafile[i].DataFile_CRC);
        LUH.write((char*)&LUH_struct.datafile[i].DataFile_CRC, sizeof(LUH_struct.datafile[i].DataFile_CRC));
        //qToBigEndian(LUH_struct.datafile[i].DataFile_LenInBytes, &LUH_struct.datafile[i].DataFile_LenInBytes);
        LUH.write((char*)&LUH_struct.datafile[i].DataFile_LenInBytes, sizeof(LUH_struct.datafile[i].DataFile_LenInBytes));
        //qToBigEndian(LUH_struct.datafile[i].DataFile_CheckVal_Len, &LUH_struct.datafile[i].DataFile_CheckVal_Len);
        LUH.write((char*)&LUH_struct.datafile[i].DataFile_CheckVal_Len, sizeof(LUH_struct.datafile[i].DataFile_CheckVal_Len));
        //qToBigEndian(LUH_struct.datafile[i].DataFile_CheckVal_Type, &LUH_struct.datafile[i].DataFile_CheckVal_Type);
        LUH.write((char*)&LUH_struct.datafile[i].DataFile_CheckVal_Type, sizeof(LUH_struct.datafile[i].DataFile_CheckVal_Type));
        LUH.write((char*)&LUH_struct.datafile[i].DataFile_CheckVal, sizeof(LUH_struct.datafile[i].DataFile_CheckVal));
    }
    LUH.flush();
    char *to_crc = (char *)malloc(LUH_struct.file_len - 2);
    LUH.seek(LUH_struct.file_len - 4);
    LUH_struct.LoadCheckVal_Type = 4;
    LUH.write((char*)&LUH_struct.LoadCheckVal_Type, sizeof(LUH_struct.LoadCheckVal_Type));
    LUH.seek(0);
    memcpy(to_crc, LUH.readAll(), LUH_struct.file_len - 2);
    LUH_struct.HFile_CRC = crc16_table(to_crc, LUH_struct.file_len - 2, 0);
    //qToBigEndian(LUH_struct.HFile_CRC, &LUH_struct.HFile_CRC);
    qDebug()<<"16进制小写输出"<<QString("%1").arg(LUH_struct.HFile_CRC,4,16,QLatin1Char('0'));
    LUH.write((char*)&LUH_struct.HFile_CRC, sizeof(LUH_struct.HFile_CRC));
    LUH.close();
    free(to_crc);
    free(LUH_struct.datafile);
}


File_LUS* UploadThread::parseLUS(QFile* fLUS)
{
    if(fLUS->open(QIODevice::ReadOnly) == false){
        return nullptr;
    }
    File_LUS *LUS = (File_LUS*)malloc(sizeof(File_LUS));
    memset(LUS, 0, sizeof(File_LUS));
    QDataStream in(fLUS);
#ifdef BIG_ENDIAN
        in.setByteOrder(QDataStream::BigEndian);
#else
        in.setByteOrder(QDataStream::LittleEndian);
#endif

    in >> LUS->file_len;
    in >> LUS->Pro_ver;
    in >> LUS->op_stat_code;
    in >> LUS->stat_des_len;
    in.readRawData(LUS->stat_des, LUS->stat_des_len);
    in >> LUS->counter;
    in >> LUS->excep_timer;
    in >> LUS->estim_timer;
    in.readRawData(LUS->load_list_ratio, 3);
    in >> LUS->hfile_num;
    if(LUS->hfile_num > 0) LUS->hfiles = (struct Hfile_info_LUS*)malloc(sizeof(Hfile_info_LUS) * LUS->hfile_num);
    for(int i = 0; i < LUS->hfile_num; ++i){
        in >> LUS->hfiles[i].fileLen;
        in >> LUS->hfiles[i].Hfile_name_len;
        in.readRawData(LUS->hfiles[i].Hfile_name, LUS->hfiles[i].Hfile_name_len + 1);
        in >> LUS->hfiles[i].load_part_num_name_len;
        in.readRawData(LUS->hfiles[i].load_part_num_name, LUS->hfiles[i].load_part_num_name_len + 1);
        in.readRawData(LUS->hfiles[i].load_ratio, 3);
        in >> LUS->hfiles[i].load_stat;
        in >> LUS->hfiles[i].load_stat_des_len;
        in.readRawData(LUS->hfiles[i].load_stat_des, LUS->hfiles[i].load_stat_des_len + 1);

        qDebug() << LUS->hfiles[i].Hfile_name_len << LUS->hfiles[i].Hfile_name << LUS->hfiles[i].load_part_num_name_len << LUS->hfiles[i].load_part_num_name
                 << LUS->hfiles[i].load_ratio << LUS->hfiles[i].load_stat << LUS->hfiles[i].load_stat_des_len << LUS->hfiles[i].load_stat_des;
    }

//    LUS->file_len = data[0] + ((uint32)data[1] << 8) + ((uint32)data[2] << 16) + ((uint32)data[3] << 24);
//    LUS->Pro_ver = data[4] + ((uint16)data[5] << 8);
//    LUS->op_stat_code = data[6] + ((uint16)data[7] << 8);
//    LUS->stat_des_len = data[8];
//    strcpy(LUS->stat_des, QString::fromStdString(data.mid(9, LUS->stat_des_len).toStdString()).toUtf8().data());
//    memcpy(LUS->load_list_ratio, data.data() + 9 + LUS->stat_des_len + 6, 3);
    qDebug() << LUS->stat_des << LUS->load_list_ratio;
    fLUS->close();
    return LUS;
}
void UploadThread::rcvStatusCodeAndMessageSlot(quint16 statusCode, QString statusMessage, bool error, QString errorMessage)
{
//    qDebug() << "rcvStatusCodeAndMessageSlot thread id" << QThread::currentThreadId();
//    conditionMutex.lock();
//    this->statusMessage = statusMessage;
//    this->statusCode = statusCode;
//    emit(uploadStatusMessage(statusMessage));
//    if(error == true){
//        status = ERROR;
//        this->errorMessage = errorMessage;
//    }
//    else{
//        switch (this->statusCode) {
//        case 0x0001:
//            status = SEND_LUR_WRQ;
//            break;
//        case 0x0002:
//            status = WAIT_FILE_RRQ;
//            break;
//        case 0x0003:
//            emit(uploadStatusMessage(QString("设备%1上传完成").arg(device->getName())));
//            emit(uploadResult(true));
//            status = END;
//            break;
//        case 0x1003:
//            status = ERROR;
//            break;
//        case 0x1004:
//            status = ERROR;
//            break;
//        case 0x1005:
//            status = ERROR;
//            break;
//        default:
//            status = ERROR;
//            errorMessage = QString(tr("未定义状态码错误"));
//            break;
//        }
//    }
//    statusFileRcved = true;
//    statusFileRcvedConditon.wakeOne();
//    conditionMutex.unlock();
}
