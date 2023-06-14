#include "uploadthread.h"

void UploadThread::run()
{
    this->tftpClient = new QUdpSocket();
    this->tftpServer = new QUdpSocket();
    this->tftpClient->connectToHost(device->getHostAddress(), 69);
    qDebug() << "上传线程的id是" << QThread::currentThreadId();
    QByteArray LUI_RRQ;
    QByteArray request;
    quint16 port;
    QString fileName;
    QFile LUR(QString("%1/%2.LUR").arg(dir.dirName(), device->getName()));
    QFile LUH(QString("%1/%2.LUH").arg(dir.dirName(), device->getName()));
    QFile *LUS;
    QString statusMessage;
    uint16 statusCodeFromLUS;
    QByteArray LUR_WRQ;
    QByteArray ack;
    int filesSent;
    bool flag = false;
    QString errorMessage;
    while(status != END){
        switch (status) {
        qDebug() << status << "now ";
        case SEND_LUI_RRQ:
            if(!Tftp::receiveFile(tftpClient, QString("%1/%2.LUI").arg(dir.dirName(), device->getName()), &errorMessage, &mainThreadExitedOrNot, Tftp::RRQ)){
                status = ERROR;
                break;
            }
            status = WAIT_LUS_WRQ;
            qDebug() << "LUI发送完成";
            emit(uploadStatusMessage(QString(tr("LUI发送完成"))));
            break;
        case WAIT_LUS_WRQ:
            request = tftpRequest->getRequest(&mainThreadExitedOrNot);
            if(request.size() == 0){
                if(mainThreadExitedOrNot){
                    errorMessage = QString(tr("主线程已退出"));
                }
                else{
                    errorMessage = QString("等待LUS写请求超时");
                }
                status = ERROR;
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
            LUS = new QFile(QString("%1/%2").arg(dir.dirName(), fileName));
            if(!LUS->open(QIODevice::ReadOnly)){
                errorMessage = QString("文件%1打开失败").arg(fileName);
                status = ERROR;
                break;
            }
            else{
                QByteArray data = LUS->readAll();
                File_LUS *LUS_struct = parseLUS(data);
                statusMessage = QString("设备%1状态信息:%2").arg(device->getName(),
                                                           QString(LUS_struct->stat_des));
                emit(uploadStatusMessage(statusMessage));
                statusCodeFromLUS = LUS_struct->op_stat_code;
                switch (statusCodeFromLUS) {
                case 0x0001:
                    status = SEND_LUR_WRQ;
                    break;
                case 0x0002:
                    status = WAIT_FILE_RRQ;
                    break;
                case 0x0003:
                    emit(uploadStatusMessage(QString("设备%1上传完成").arg(device->getName())));
                    emit(uploadResult(true));
                    status = END;
                    break;
                case 0x1003:
                    status = END;
                    break;
                case 0x1004:
                    status = END;
                    break;
                case 0x1005:
                    status = END;
                    break;
                default:
                    status = ERROR;
                    errorMessage = QString(tr("未定义状态码错误"));
                    break;
                }
                LUS->close();
                free(LUS_struct);
            }
            break;
        case SEND_LUR_WRQ:
            makeLUR();
            if(!LUR.exists()){
                errorMessage = QString("LUR文件创建失败");
                status = ERROR;
                break;
            }
            if(!Tftp::sendFile(tftpClient, QString("%1/%2.LUR").arg(dir.dirName(), device->getName()), &errorMessage, &mainThreadExitedOrNot, Tftp::WRQ)){
                status = ERROR;
                break;
            }
            qDebug() << "LUR发送完成";
            emit(uploadStatusMessage("LUR发送完成"));
            status = WAIT_LUH_RRQ;
            break;
        case WAIT_LUH_RRQ:
            request = tftpRequest->getRequest(&mainThreadExitedOrNot);
            if(request.size() == 0){
                status = ERROR;
                errorMessage = mainThreadExitedOrNot ? QString(tr("主线程已退出")) : QString("等待LUH文件读请求超时");
                break;
            }
            port = tftpRequest->getPort();
            fileName = request.mid(2).split('\0').at(0);
            if(fileName.endsWith(".LUS")){
                status = WAIT_LUS_WRQ;
                break;
            }
            tftpRequest->lockMutex();
            tftpServer->disconnectFromHost();
            tftpServer->connectToHost(device->getHostAddress(), port);
            if(fileName.endsWith(".LUH")){
                makeLUH();
                if(!LUH.exists()){
                    status = ERROR;
                    errorMessage = QString("LUH文件创建失败");
                    break;
                }
                if(!Tftp::sendFile(tftpServer, QString("%1\\%2.LUH").arg(dir.dirName(), device->getName()), &errorMessage, &mainThreadExitedOrNot, Tftp::RRQ)){
                    status = ERROR;
                    break;
                }
            }
            else{
                status = ERROR;
                errorMessage = QString("未知文件请求错误");
                break;
            }
            qDebug() << "LUH发送完成";
            emit(uploadStatusMessage("LUH发送完成"));
            status = WAIT_FILE_RRQ;
            break;
        case WAIT_FILE_RRQ:
            request = tftpRequest->getRequest(&mainThreadExitedOrNot);
            if(request.size() == 0){
                status = ERROR;
                errorMessage = mainThreadExitedOrNot ? QString(tr("主线程已退出")) : QString("等待上传文件读请求超时");
                break;
            }
            port = tftpRequest->getPort();
            fileName = request.mid(2).split('\0').at(0);
            if(fileName.endsWith(".LUS")){
                status = WAIT_LUS_WRQ;
                break;
            }
            tftpRequest->lockMutex();
            tftpServer->disconnectFromHost();
            tftpServer->connectToHost(device->getHostAddress(), port);
            for(int i = 0; i < fileList.size(); i++){
                if(fileList.at(i).contains(fileName)){
                    if(!Tftp::sendFile(tftpServer, fileList.at(i), &errorMessage, &mainThreadExitedOrNot, Tftp::RRQ)){
                        status = ERROR;
                        flag = true;
                    }
                    break;
                }
            }
            if(flag) break;
            if(!files_sent[fileName]) fileSentCnt++;
            emit(uploadRate(fileSentCnt * 100 / fileList.size(), true));
            emit(uploadStatusMessage(QString("设备%1: 文件%2上传完成.(%3/%4)")
                                        .arg(device->getName())
                                        .arg(fileName)
                                        .arg(fileSentCnt)
                                        .arg(fileList.size())));
            files_sent[fileName] = true;
            break;
        case ERROR:
            emit(uploadStatusMessage(errorMessage));
            emit(uploadStatusMessage(QString(tr("上传操作异常结束"))));
            emit(uploadResult(false));
            emit(uploadRate(0, false));
            status = END;
            break;
        case END:
            break;

        default:
            break;
        }
    }
    emit(threadFinish(UPLOAD_OP_CODE, QString(tr("上传操作结束"))));
    qDebug() << "mainThreadExitedOrNot状态" << mainThreadExitedOrNot;
    if(subOfAuto) tftpRequest = nullptr;
}

void UploadThread::makeLUR(){
    QFile LUR(QString("%1/%2.LUR").arg(dir.dirName(), device->getName()));
    LUR.open(QIODevice::WriteOnly);
    File_LUR LUR_struct;
    memcpy(LUR_struct.Pro_ver, Protocol_ver, 2);
    LUR_struct.Header_num = 1;
    LUR_struct.file_len = sizeof(LUR_struct.Pro_ver) + sizeof(LUR_struct.Header_num) + sizeof(LUR_struct.file_len);
    LUR_struct.Hfile = (HFILE_INFO*) malloc(LUR_struct.Header_num*sizeof (HFILE_INFO));
    for(int i = 0; i < LUR_struct.Header_num; i++){
        memset(LUR_struct.Hfile[i].name, 0 ,sizeof (LUR_struct.Hfile[i].name));
        strcpy(LUR_struct.Hfile[i].name, QString("%1.LUH").arg(device->getName()).toUtf8().data());
        LUR_struct.Hfile[i].len_name = strlen(LUR_struct.Hfile[i].name) + 1;
        LUR_struct.file_len += sizeof(LUR_struct.Hfile[i].len_name) + LUR_struct.Hfile[i].len_name;
        memset(LUR_struct.Hfile[i].load_part_name, 0, sizeof(LUR_struct.Hfile[i].load_part_name));
        strcpy(LUR_struct.Hfile[i].load_part_name, dir.absolutePath().toUtf8().data());
        LUR_struct.Hfile[i].load_part_len_name = strlen(LUR_struct.Hfile[i].load_part_name) + 1;
        LUR_struct.file_len += sizeof(LUR_struct.Hfile[i].load_part_len_name) + LUR_struct.Hfile[i].load_part_len_name;
    }
    LUR.write((char *)&LUR_struct.file_len, sizeof(LUR_struct.file_len));
    LUR.write(LUR_struct.Pro_ver, sizeof(LUR_struct.Pro_ver));
    LUR.write((char *)&LUR_struct.Header_num, sizeof(LUR_struct.Header_num));
    for(int i = 0; i < LUR_struct.Header_num; i++){
        LUR.write((char *)&LUR_struct.Hfile[i].len_name, sizeof(LUR_struct.Hfile[i].len_name));
        LUR.write(LUR_struct.Hfile[i].name, LUR_struct.Hfile[i].len_name);
        LUR.write((char *)&LUR_struct.Hfile[i].load_part_len_name, sizeof(LUR_struct.Hfile[i].load_part_len_name));
        LUR.write(LUR_struct.Hfile[i].load_part_name, LUR_struct.Hfile[i].load_part_len_name);
    }
    LUR.close();
    free(LUR_struct.Hfile);
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
//            char* to_crc = (char*)malloc(file.size() * sizeof (char));
//            memcpy(to_crc, LUH.readAll().data(), file.size());
//            qDebug() << file.size() << "file size";
//            LUH_struct.datafile[i].DataFile_CRC = crc16_table(to_crc, file.size(), 0);
            unsigned short crc = 0;
            size_t nBytes;
            while((nBytes = file.read(to_crc, 512))){
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
    char *to_crc = (char * ) malloc(LUH_struct.file_len - 2);
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


File_LUS* UploadThread::parseLUS(QByteArray data)
{
    File_LUS *LUS = (File_LUS*)malloc(sizeof(File_LUS));
    LUS->file_len = data[0] + ((uint32)data[1] << 8) + ((uint32)data[2] << 16) + ((uint32)data[3] << 24);
    LUS->Pro_ver = data[4] + ((uint16)data[5] << 8);
    LUS->op_stat_code = data[6] + ((uint16)data[7] << 8);
    LUS->stat_des_len = data[8];
    strcpy(LUS->stat_des, QString::fromStdString(data.mid(9, LUS->stat_des_len).toStdString()).toUtf8().data());
    qDebug() << LUS->stat_des;
    return LUS;
}







