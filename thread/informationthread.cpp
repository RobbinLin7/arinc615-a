#include "informationthread.h"
#include <QDir>
#include<QDebug>
#include<iostream>


void InformationThread::run(){
    this->tftpClient = new QUdpSocket();
    this->tftpServer = new QUdpSocket();
    QString errorMessage;
    QByteArray request;
    quint16 port;
    QString fileName;
    while(status != END){
        switch (status) {
        case SEND_LCI_RRQ:
            if(!Tftp::get(tftpClient, dir.dirName(), QString("%1.LCI").arg(device->getName()), &errorMessage, QHostAddress(device->getHostAddress()), 69)){
                status = ERROR;
                break;
            }
            status = WAIT_LCL_WRQ;
            emit(informationStatusMessage(QString(tr("LCI发送完成"))));
            break;
        case WAIT_LCL_WRQ:
            request = tftpRequest->getRequest(&mainThreadExitedOrNot);
            if(request.size() == 0){
                status = ERROR;
                errorMessage = QString("等待LCL文件写请求超时");
                break;
            }
            port = tftpRequest->getPort();
            fileName = request.mid(2).split('\0').at(0);
            if(fileName.split('.').size() == 2 && fileName.split('.').at(1) == "LCL"){
                if(!Tftp::handlePut(tftpServer, dir.dirName(), fileName, &errorMessage, QHostAddress(device->getHostAddress()), port, request)){
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
            return;
        case END:
            break;
        }
    }
}

File_LCL* InformationThread::parseLCL(){
    QFile LCL(QString("%1/%2.LCL").arg(dir.dirName(), device->getName()));
    File_LCL *LCL_struct = (File_LCL*)malloc(sizeof(File_LCL));
    if(LCL.open(QIODevice::ReadOnly)){
        qDebug() << "文件打开成功";
        QDataStream in(&LCL);
        in.setByteOrder(QDataStream::BigEndian);
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
            in.readRawData(LCL_struct->Hws[i].target_code, LCL_struct->Hws[i].target_code_length);
            qDebug() << LCL_struct->Hws[i].target_code << "target_code";
            in >> LCL_struct->Hws[i].serial_length;
            in.readRawData(LCL_struct->Hws[i].serial_code, LCL_struct->Hws[i].serial_length);
            in >> LCL_struct->Hws[i].number_part_number;
            const unsigned int partsNo = LCL_struct->Hws[i].number_part_number;
            LCL_struct->Hws[i].parts = (partInfo*)malloc(sizeof(partInfo) * partsNo);
            for(unsigned int j = 0; j < partsNo; j++){
                in >> LCL_struct->Hws[i].parts[j].part_number_length;
                in.readRawData(LCL_struct->Hws[i].parts[j].part_number, LCL_struct->Hws[i].parts[j].part_number_length);
                in >> LCL_struct->Hws[i].parts[j].part_design_length;
                in.readRawData(LCL_struct->Hws[i].parts[j].part_design, LCL_struct->Hws[i].parts[j].part_design_length);
                in >> LCL_struct->Hws[i].parts[j].amend_length;
                in.readRawData(LCL_struct->Hws[i].parts[j].amend, LCL_struct->Hws[i].parts[j].amend_length);
            }
        }
        LCL.close();
        return LCL_struct;
    }
}
