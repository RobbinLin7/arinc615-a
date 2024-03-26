#include "uploadthread.h"

void UploadThread::run()
{
    auto debug = [&](std::string log){
        SPDLOG_LOGGER_DEBUG(&logger, "[UPLOAD][DNAME:{0}][IP:{1}]: {2}", device->getName().toStdString(),
                            device->getHostAddress().toStdString(), log);
    };
    protocalFileSocket = std::make_shared<QUdpSocket>();
    if(protocalFileSocket->bind(PROTOCAL_FILE_PORT) == false){
        debug(QString("端口号%1被占用").arg(PROTOCAL_FILE_PORT).toStdString());
        return;
    }
    QByteArray request;
    quint16 port;
    QString fileName;
    QFile LUR(QString("%1/%2.LUR").arg(dir.dirName(), device->getName()));
    QFile LUH(QString("%1/%2.LUH").arg(dir.dirName(), device->getName()));
    //bool flag = false;
    const unsigned short DLP_retry = 2;
    unsigned short tries = 0;
    QString errorMessage;
    while(status != END){
        //waitTimes = 0;
        switch (status) {
        case INITIALIZATION:
            qDebug() << QString("%1/%2.LUR").arg(dir.dirName(), device->getName());
            while(!Tftp::get(protocalFileSocket.get(), dir.dirName(), QString("%1.LUI").arg(device->getName()), &errorMessage, QHostAddress(device->getHostAddress()), TFTP_SERVER_PORT) &&
                  ++tries < DLP_retry + 1){
                emit(uploadStatusMessage(errorMessage));
            }
            if(tries >= DLP_retry + 1){
                status = ERROR_;
                errorMessage = QString("请求LUI文件失败");
                break;
            }
            tries = 0;

            debug("LUI发送完成");
            status = LIST_TRANSFER;

            break;
        case LIST_TRANSFER:{
            std::unique_lock<std::mutex> lock(m);
            while(flag == false){
                cv.wait(lock);
            }
            flag = false;
            if(LUS.op_stat_code != 0x0001){
                break;
            }
            //当状态码为0x0001时表示设备端接受上传操作
            makeLUR();
            if(!LUR.exists()){
                errorMessage = QString("LUR文件创建失败");
                status = ERROR_;
                break;
            }
            if(!Tftp::put(protocalFileSocket.get(), dir.dirName(), QString("%1.LUR").arg(device->getName()), &errorMessage, QHostAddress(device->getHostAddress()), TFTP_SERVER_PORT)){
                status = ERROR_;
                break;
            }
            debug("LUR文件上传完成");
            status = TRANSFER;
            break;
        }
        case TRANSFER:{
            if(tftpRequest->mutex.tryLock(13 * 1000) == false){
                status = ERROR_;
                errorMessage = QString("等待数据文件读请求超时");
                break;
            }
            request = tftpRequest->getRequest();
            port = tftpRequest->getPort();
            fileName = request.mid(2).split('\0').at(0);
            for(int i = 0; i < fileList.size(); ++i){
                if(fileList.at(i).contains(fileName)){
                    if(Tftp::handleGet(protocalFileSocket.get(), fileList.at(i).left(fileList.at(i).lastIndexOf('/')), fileName, &errorMessage, QHostAddress(device->getHostAddress()), port, request) == false){
                        debug("upload file " + fileName.toStdString() + " error");
                    }
                    else {
                        ++fileSentCnt;
                        debug(QString("文件%1上传完成").arg(fileName).toStdString());
                        emit(uploadRate(fileSentCnt * 100 / fileList.size(), true));
                    }
                    break;
                }
            }
            //等待状态文件的指示，判断操作还在进行，是出现错误，还是完成
            std::unique_lock<std::mutex> lock(m);
            do{
                cv.wait(lock);
            }while(flag == false);
            flag = false;
            switch(LUS.op_stat_code){
            case 0x0002:
                break;
            case 0x0003:
                status = END;
                break;
            default:
                status = END;
                break;
            }
            break;
        }
        case ERROR_:
            //TODO----执行Abort操作
            debug(errorMessage.toStdString());
            status = END;
            break;
        default:
            break;
        }
    }
    debug("上传操作结束");
    protocalFileSocket->close();
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


void UploadThread::parseStatusFile()
{
    free(LUS.hfiles);
    QFile fLUS(QString("%1/%2.LUS").arg(dir.dirName()).arg(device->getName()));
    if(fLUS.open(QIODevice::ReadOnly) == false){
        return;
    }
    memset(&LUS, 0, sizeof(File_LUS));
    QDataStream in(&fLUS);
#ifdef BIG_ENDIAN
        in.setByteOrder(QDataStream::BigEndian);
#else
        in.setByteOrder(QDataStream::LittleEndian);
#endif

    in >> LUS.file_len;
    in.readRawData(LUS.Pro_ver, 2);
    in >> LUS.op_stat_code;
    in >> LUS.stat_des_len;
    in.readRawData(LUS.stat_des, LUS.stat_des_len);
    in >> LUS.counter;
    in >> LUS.excep_timer;
    in >> LUS.estim_timer;
    in.readRawData(LUS.load_list_ratio, 3);
    in >> LUS.hfile_num;
    if(LUS.hfile_num > 0) LUS.hfiles = (struct Hfile_info_LUS*)malloc(sizeof(Hfile_info_LUS) * LUS.hfile_num);
    for(int i = 0; i < LUS.hfile_num; ++i){
        in >> LUS.hfiles[i].fileLen;
        in >> LUS.hfiles[i].Hfile_name_len;
        in.readRawData(LUS.hfiles[i].Hfile_name, LUS.hfiles[i].Hfile_name_len + 1);
        in >> LUS.hfiles[i].load_part_num_name_len;
        in.readRawData(LUS.hfiles[i].load_part_num_name, LUS.hfiles[i].load_part_num_name_len + 1);
        in.readRawData(LUS.hfiles[i].load_ratio, 3);
        in >> LUS.hfiles[i].load_stat;
        in >> LUS.hfiles[i].load_stat_des_len;
        in.readRawData(LUS.hfiles[i].load_stat_des, LUS.hfiles[i].load_stat_des_len + 1);

//        qDebug() << LUS.hfiles[i].Hfile_name_len << LUS.hfiles[i].Hfile_name << LUS.hfiles[i].load_part_num_name_len << LUS.hfiles[i].load_part_num_name
//                 << LUS.hfiles[i].load_ratio << LUS.hfiles[i].load_stat << LUS.hfiles[i].load_stat_des_len << LUS.hfiles[i].load_stat_des;
    }
    fLUS.close();
    std::unique_lock<std::mutex> lock(m);
    flag = true;
    cv.notify_one();
    emit(parseStatusFileFinished(LUS));
    return;
}

