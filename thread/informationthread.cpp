#include "informationthread.h"
#include <QDir>
#include<QDebug>
#include<iostream>


void InformationThread::run(){
    protocalFileSocket = std::make_shared<QUdpSocket>();
    if(protocalFileSocket->bind(PROTOCAL_FILE_PORT) == false){
        qDebug() << QString("端口号%1被占用").arg(PROTOCAL_FILE_PORT);
        return;
    }
    QString errorMessage;
    QByteArray request;
    quint16 port;
    QString fileName;
    while(status != END){
        switch (status) {
        case SEND_LCI_RRQ:
            if(!Tftp::get(protocalFileSocket.get(), dir.dirName(), QString("%1.LCI").arg(device->getName()), &errorMessage, QHostAddress(device->getHostAddress()), TFTP_SERVER_PORT)){
                status = ERROR;
                break;
            }
            status = WAIT_LCL_WRQ;
            emit(informationStatusMessage(QString(tr("LCI发送完成"))));
            break;
        case WAIT_LCL_WRQ:
            if(tftpRequest->mutex.tryLock(13 * 1000) == false){
                status = ERROR;
                errorMessage = QString("等待LCL写请求超时");
                break;
            }
            request = tftpRequest->getRequest();
            port = tftpRequest->getPort();
            qDebug() << "port = " << port << "2";
            tftpRequest->mutex.unlock();
            fileName = request.mid(2).split('\0').at(0);
            if(fileName.split('.').size() == 2 && fileName.split('.').at(1) == "LCL"){
                if(!Tftp::handlePut(protocalFileSocket.get(), dir.dirName(), fileName, &errorMessage, QHostAddress(device->getHostAddress()), port, request)){
                    status = ERROR;
                    break;
                }
                qDebug() << "LNL receive complete";
                emit(informationStatusMessage(QString(tr("LCL接收完成"))));
                File_LCL *LCL_struct = parseLCL();
                emit(informationFinished(LCL_struct, device->getName(), device->getHostAddress()));
                status = END;
            }
            else{
                status = ERROR;
                errorMessage = QString("收到非LCL文件请求");
            }
            break;
        case ERROR:
            status = END;
            emit(informationStatusMessage(errorMessage));
            emit(informationStatusMessage(QString("设备%1信息操作失败").arg(device->getName())));
            emit(informationFinished(NULL, device->getName(), device->getHostAddress()));
            break;
        case END:
            break;
        }
    }
    protocalFileSocket->close();
}

File_LCL* InformationThread::parseLCL(){
    QFile LCL(QString("%1/%2.LCL").arg(dir.dirName(), device->getName()));
    File_LCL *LCL_struct = (File_LCL*)malloc(sizeof(File_LCL));
    if(LCL.open(QIODevice::ReadOnly)){
        qDebug() << "文件打开成功";
        QDataStream in(&LCL);
        //in.setByteOrder(QDataStream::BigEndian);
#ifdef BIG_ENDIAN
        in.setByteOrder(QDataStream::BigEndian);
#else
        in.setByteOrder(QDataStream::LittleEndian);
#endif
        in >> LCL_struct->file_len;
        qDebug() << LCL_struct->file_len << "file_len";
        in.readRawData(LCL_struct->Pro_ver, 2);
        qDebug() << LCL_struct->Pro_ver << "Pro_ver";
        in >> LCL_struct->Hw_num;
        qDebug() << LCL_struct->Hw_num << "Hw_num";
        LCL_struct->Hws = (targetInfo*)malloc(sizeof(targetInfo) * LCL_struct->Hw_num);
        for(int i = 0; i < LCL_struct->Hw_num; i++){
            in >> LCL_struct->Hws[i].target_code_length;
            qDebug() << LCL_struct->Hws[i].target_code_length << "target_code_length";
            in.readRawData(LCL_struct->Hws[i].target_code, LCL_struct->Hws[i].target_code_length + 1); //要读入'\0'
            qDebug() << LCL_struct->Hws[i].target_code << "target_code";
            in >> LCL_struct->Hws[i].serial_length;
            in.readRawData(LCL_struct->Hws[i].serial_code, LCL_struct->Hws[i].serial_length + 1);
            in >> LCL_struct->Hws[i].number_part_number;
            const unsigned int partsNo = LCL_struct->Hws[i].number_part_number;
            LCL_struct->Hws[i].parts = (partInfo*)malloc(sizeof(partInfo) * partsNo);
            for(unsigned int j = 0; j < partsNo; j++){
                in >> LCL_struct->Hws[i].parts[j].part_number_length;
                in.readRawData(LCL_struct->Hws[i].parts[j].part_number, LCL_struct->Hws[i].parts[j].part_number_length + 1);
                in >> LCL_struct->Hws[i].parts[j].part_design_length;
                in.readRawData(LCL_struct->Hws[i].parts[j].part_design, LCL_struct->Hws[i].parts[j].part_design_length + 1);
                in >> LCL_struct->Hws[i].parts[j].amend_length;
                in.readRawData(LCL_struct->Hws[i].parts[j].amend, LCL_struct->Hws[i].parts[j].amend_length + 1);
            }
        }
        LCL.close();
        return LCL_struct;
    }
}

void InformationThread::parseStatusFile()
{
    QFile statusFile(QString("%1/%2.LCS").arg(dir.dirName(), device->getName()));
    if(!statusFile.open(QIODevice::ReadOnly)){
        qDebug() << QString("%1/%2.LCS打开失败").arg(dir.dirName(), device->getName());
        return;
    }
    QDataStream in(&statusFile);
#ifdef BIG_ENDIAN
    in.setByteOrder(QDataStream::BigEndian);
#else
    in.setByteOrder(QDataStream::LittleEndian);
#endif
    in >> LCS.file_len;
    in.readRawData(LCS.Pro_ver, 2);
    in >> LCS.counter;
    in >> LCS.statusCode;
    in >> LCS.exceptionTimer;
    in >> LCS.estimatedTime;
    in >> LCS.stat_des_len;
    in.readRawData(LCS.stat_des, LCS.stat_des_len + 1);
    qDebug() << "LCS statusCode" << LCS.statusCode;
    qDebug() << LCS.stat_des;
}
