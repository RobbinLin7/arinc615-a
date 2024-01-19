#include "tftp.h"
#include <algorithm>
QByteArray Tftp::makeTftpReadRequest(QString fileName, QString mode, quint16 valueOfBlockSize, quint16 valueOfTimeOut)
{
    QByteArray request;
    //1.opcode
    request.append('\0');
    request.append(1);
    //2.fileName
    request.append(fileName);
    request.append('\0');
    //3.mode
    request.append(mode);
    request.append('\0');
    //4.blocksize 范围为8-2^14
    request.append("blksize");
    request.append('\0');
    request.append(QString::number(valueOfBlockSize));
    request.append('\0');
    //5.timeout 范围为1-65535
    request.append("timeout");
    request.append('\0');
    request.append(QString::number(valueOfTimeOut));
    request.append('\0');
    return request;
}


QByteArray Tftp::makeTftpWriteRequest(QString fileName, QString mode, quint16 valueOfBlockSize, quint16 valueOfTimeOut)
{
    QByteArray request;
    //1.opcode
    request.append('\0');
    request.append(2);

    //2.fileName
    request.append(fileName);
    request.append('\0');

    //3.mode
    request.append(mode);
    request.append('\0');

    //4.blocksize 范围为8-65464

    request.append("blksize");
    request.append('\0');
    request.append(QString::number(valueOfBlockSize).toStdString().c_str());
    request.append('\0');

    //5.timeout 范围为1-65535

    request.append("timeout");
    request.append('\0');
    request.append(QString::number(valueOfTimeOut));
    request.append('\0');

    return request;
}

QByteArray Tftp::makeTftpAck(quint16 block)
{
    QByteArray ack;

    //1.opcode
    ack.append('\0');
    ack.append(4);

    //2.block
    ack.append(block >> 8);
    ack.append(block);

    return ack;
}


QByteArray Tftp::makeTftpError(quint16 errorCode, QString errorMessage)
{
    QByteArray error;
    //1.opcode
    error.append('\0');
    error.append(5);
    //2.errorCode
    error.append(errorCode >> 8);
    error.append(errorCode);
    //3.errorMessage
    error.append(errorMessage);
    error.append('\0');

    return error;
}

bool Tftp::checkDataBlockNo(const QByteArray &tftpPacket, unsigned short expectedBlockNo)
{
    if(getTftpPacketType(tftpPacket) != DATA || tftpPacket.size() < 4){
        return false;
    }
    unsigned short high = static_cast<unsigned char>(tftpPacket[2]);
    unsigned short low = static_cast<unsigned char>(tftpPacket[3]);
    unsigned short blockNo = (high << 8) + low;
    return blockNo == expectedBlockNo;
}

QByteArray Tftp::makeTftpOAck(const QMap<QString, QString> &options)
{
    QByteArray oack;

    //1.opcode
    oack.append('\0');
    oack.append(6);

    //2.every option and value
    for(auto it = options.constBegin(); it != options.constEnd(); ++it){
        oack.append(it.key());
        oack.append('\0');
        oack.append(it.value());
        oack.append('\0');
    }

    return oack;
}

QByteArray Tftp::makeTftpOAck(std::initializer_list<std::pair<std::string, std::string> > options)
{
    QByteArray oack;

    //1.opcode
    oack.append('\0');
    oack.append(6);

    //2.every option and value
    for(auto it = options.begin(); it != options.end(); ++it){
        oack.append(it->first.data());
        //oack.append('\0');
        oack.append(it->second.data());
        //oack.append('\0');
    }
    return oack;
}


bool Tftp::get(QUdpSocket *uSock, QString path, QString fileName, QString *errorMessage, const QHostAddress& host, const quint16 port)
{
    QByteArray readRequest = makeTftpReadRequest(fileName);
    QByteArray response, ack;
    unsigned short retransTimes = 0;
    //1.consultation stage

    //1.1 send RRQ with options
    do{
        uSock->writeDatagram(readRequest, host, port);
    }while(retransTimes++ < max_retrans_times &&
           (!uSock->waitForReadyRead(timeout * 1000) ||
            !uSock->readDatagram(response.data(), uSock->pendingDatagramSize()) ||
            getTftpPacketType(response) != OACK));
    if(retransTimes == max_retrans_times){
        *errorMessage = QString("等待OACK报文超时");
        return false;
    }

    //1.2 receive and parse OACK
    auto index = response.indexOf("blksize");
    if(index != -1){
        index += sizeof("blksize");
        blkSize = 0;
        while(static_cast<char>(response[index]) != '\0'){
            blkSize = blkSize * 10 + static_cast<char>(response[index]) - '0';
            ++index;
        }
    }
    index = response.indexOf("timeout");
    if(index != -1){
        index += sizeof("timeout");
        timeout = 0;
        while(static_cast<char>(response[index]) != '\0'){
            timeout = timeout * 10 + static_cast<char>(response[index]) - '0';
            ++index;
        }
    }

    //1.3 send ACK 0
    ack = makeTftpAck(0);
    uSock->writeDatagram(readRequest, host, port);



    //2.data transfer stage
    return download(uSock, path, fileName, errorMessage, host, port, ack);

}

bool Tftp::handlePut(QUdpSocket *uSock, QString path, QString fileName, QString *errorMessage, const QHostAddress &host, const quint16 port, QByteArray writeRequest)
{
    //1.consulting stage
    auto index = writeRequest.indexOf("blksize");
    int blkSizeFromRequest = -1, timeoutFromRequest = -1;
    if(index != -1){
        index += sizeof("blksize");
        blkSizeFromRequest = 0;
        while(static_cast<char>(writeRequest[index]) != '\0'){
            blkSizeFromRequest = blkSizeFromRequest * 10 + static_cast<char>(writeRequest[index]) - '0';
            ++index;
        }
        //如果blkSizeFromRequest在范围内
        blkSize = std::min(blkSize, static_cast<unsigned short>(blkSizeFromRequest));
    }
    index = writeRequest.indexOf("timeout");
    if(index != -1){
        index += sizeof("timeout");
        timeoutFromRequest = 0;
        while(static_cast<char>(writeRequest[index]) != '\0'){
            timeoutFromRequest = timeoutFromRequest * 10 + static_cast<char>(writeRequest[index]) - '0';
            ++index;
        }
        //如果timeoutFromRequest在范围内
        timeout = std::min(timeout, static_cast<unsigned short>(timeoutFromRequest));
    }
    QByteArray oack = makeTftpOAck({std::make_pair(std::string("blkSize"), std::to_string(blkSize))});
    uSock->writeDatagram(oack, host, port);
    return download(uSock, path, fileName, errorMessage, host, port, oack);
}

bool Tftp::download(QUdpSocket *uSock, QString path, QString fileName, QString *errorMessage, const QHostAddress &host, const quint16 port, QByteArray lastPacketOfConsult)
{
    QFile file(path + "/" + fileName);
    if(file.open(QIODevice::WriteOnly) == false){
        *errorMessage = QString("文件打开失败%1").arg(fileName);
        return false;
    }
    QByteArray previousPacekt = lastPacketOfConsult;
    QByteArray dataPacket;
    unsigned short transTimes = 0;
    unsigned short dataLen = 0;
    unsigned short expectedBlockNo = 1;
    //while(uSock)
    do{
        while(transTimes++ < max_retrans_times &&
              (!uSock->waitForReadyRead(timeout * 1000) ||
               !(dataLen = uSock->readDatagram(dataPacket.data(), uSock->pendingDatagramSize())) ||
               !checkDataBlockNo(dataPacket, expectedBlockNo))){
            uSock->writeDatagram(previousPacekt, host, port);
        }
        if(transTimes == max_retrans_times){
            *errorMessage = QString("等待DATA报文超时");
            return false;
        }
        file.write(dataPacket.mid(4));
        QByteArray ack = makeTftpAck(expectedBlockNo++);
        uSock->writeDatagram(ack, host, port);
        previousPacekt = ack;
    }while(dataLen == blkSize);
    file.close();
    return true;
}


QByteArray Tftp::makeTftpData(char data[], int len, quint16 block){
    QByteArray tftpData;

    //1.opcode
    tftpData.append('\0');
    tftpData.append(3);

    //2.blcok
    tftpData.append(block >> 8);
    tftpData.append(block);

    //3.data
    tftpData.append(data, len);
    return tftpData;
}





//bool Tftp::sendFile(QUdpSocket *uSock, QString path, QString *errorMessage, bool *mainThreadExitedOrNot, TftpMode tftpMode)
//{
//    QString fileName = path.lastIndexOf('/') < path.size() ? path.mid(path.lastIndexOf('/') + 1) : path;
//    QFile file(path);
//    if(!file.open(QIODevice::ReadOnly)){
//        *errorMessage = QString("文件%1打开失败").arg(fileName);
//        return false;
//    }
//    int dataLen = -1;
//    quint16 block = 1;
//    char data[TFTP_NOT_LAST_DATA_LEN];
//    memset(data, 0, sizeof(data));
//    while(tftpMode == WRQ){
//        unsigned int retrans_times = 0;
//        QByteArray wrq = makeTftpWriteRequest(fileName);
//        bool readyRead = false;
//        do{
//            uSock->write(wrq.data(), wrq.size());
//            retrans_times++;
//            readyRead = uSock->waitForReadyRead(wait_time_ms);
//        }while(!(*mainThreadExitedOrNot) && !readyRead && retrans_times < max_retrans_times);
//        if(*mainThreadExitedOrNot){
//            *errorMessage = QString("主线程已退出");
//            return false;
//        }
//        if(!readyRead){
//            *errorMessage = QString("%1写请求相应超时").arg(fileName);
//            return false;
//        }
//        QByteArray ack;
//        ack.resize(uSock->pendingDatagramSize());
//        uSock->read(ack.data(), ack.size());
//        if(!strcmp(ack.left(4).data(), "\x00\x04\x00\x00")) break;
//        else{
//            *errorMessage = QString("设备端拒绝了你的%1写请求").arg(fileName);
//            return false;
//        }
//    }
//    do{
//        QByteArray tftpData, ack;
//        unsigned int retrans_times = 0;
//        dataLen = file.read(data, TFTP_NOT_LAST_DATA_LEN);
//        tftpData = makeTftpData(data, dataLen, block);
//        bool readyRead = false;
//        quint16 ackNo = 0;
//        while(!(*mainThreadExitedOrNot) && (!readyRead || block != ackNo) && retrans_times <= max_retrans_times){
//            uSock->write(tftpData.data(), tftpData.size());
//            readyRead = uSock->waitForReadyRead(wait_time_ms);//设置2秒的超时时间,最多重传20次
//            if(readyRead){
//                //qDebug() << uSock->pendingDatagramSize() << uSock->bytesAvailable();
//                if(uSock->pendingDatagramSize() < 4){

//                    ack.resize(uSock->bytesAvailable());
//                    uSock->read(ack.data(), ack.size());
//                    //qDebug() << ack << uSock->pendingDatagramSize() << uSock->bytesAvailable();
//                    *errorMessage = QString("%1:设备端端口不可达").arg(fileName);
//                    return false;
//                }
//                ack.resize(uSock->pendingDatagramSize());
//                uSock->read(ack.data(), ack.size());
//                quint16 high = (unsigned char)ack.at(2);
//                quint16 low = (unsigned char)ack.at(3);
//                ackNo = (high << 8) + low;
//            }
//            retrans_times++;
//        }
//        if(*mainThreadExitedOrNot){
//            *errorMessage = QString("主线程已退出");
//            return false;
//        }
//        if(retrans_times > max_retrans_times){
//            *errorMessage = QString("等待%1:ACK数据包超时").arg(fileName);
//            return false;
//        }
//        block++;
//    }while(dataLen == TFTP_NOT_LAST_DATA_LEN);
//    file.close();
//    return true;
//}

//bool Tftp::sendFile_GBN(QUdpSocket *uSock, QString path, QString *errorMessage, bool *mainThreadExitedOrNot, TftpMode tftpMode)
//{
//    QString fileName = path.lastIndexOf('/') < path.size() ? path.mid(path.lastIndexOf('/') + 1) : path;
//    QFile file(path);
//    if(!file.open(QIODevice::ReadOnly)){
//        *errorMessage = QString("文件%1打开失败").arg(fileName);
//        return false;
//    }
//    int dataLen = -1;
//    quint16 block = 1;
//    char data[TFTP_NOT_LAST_DATA_LEN];
//    memset(data, 0, sizeof(data));
//    while(tftpMode == WRQ){
//        unsigned int retrans_times = 0;
//        QByteArray wrq = makeTftpWriteRequest(fileName);
//        bool readyRead = false;
//        do{
//            uSock->write(wrq.data(), wrq.size());
//            retrans_times++;
//            readyRead = uSock->waitForReadyRead(wait_time_ms);
//        }while(!(*mainThreadExitedOrNot) && !readyRead && retrans_times < max_retrans_times);
//        if(*mainThreadExitedOrNot){
//            *errorMessage = QString("主线程已退出");
//            return false;
//        }
//        if(!readyRead){
//            *errorMessage = QString("%1写请求相应超时").arg(fileName);
//            return false;
//        }
//        QByteArray ack;
//        ack.resize(uSock->pendingDatagramSize());
//        uSock->read(ack.data(), ack.size());
//        if(!strcmp(ack.left(4).data(), "\x00\x04\x00\x00")) break;
//        else{
//            *errorMessage = QString("设备端拒绝了你的%1写请求").arg(fileName);
//            return false;
//        }
//    }
//    unsigned short base = 1, nextseqnum = 1;
//    const unsigned short N = 16; //滑动窗口大小
//    unsigned short retrans_times = 0;
//    Packet packet[N]; //缓存和滑动窗口个数相当的packet
//    QByteArray ack;
//    while(nextseqnum < base + N){
//        packet[nextseqnum % N].dataLen = file.read(data, TFTP_NOT_LAST_DATA_LEN);
//        packet[nextseqnum % N].data = makeTftpData(data, packet[nextseqnum % N].dataLen, nextseqnum);
//        uSock->write(packet[nextseqnum % N].data.data(), packet[nextseqnum % N].data.size());
//        if(packet[nextseqnum++ % N].dataLen < TFTP_NOT_LAST_DATA_LEN) break;
//    }
//    while(!(*mainThreadExitedOrNot) && retrans_times < max_retrans_times){
//        bool readReady = uSock->waitForReadyRead(wait_time_ms);
//        if(!readReady){
//            for(unsigned short i = base; i != nextseqnum; i++){
//                uSock->write(packet[i % N].data.data(), packet[i % N].data.size());
//            }
//            retrans_times++;
//            continue;
//        }
//        ack.resize(uSock->pendingDatagramSize());
//        uSock->read(ack.data(), ack.size());
//        quint16 high = (unsigned char)ack.at(2);
//        quint16 low = (unsigned char)ack.at(3);
//        block = (high << 8) + low;
//        //block = ((unsigned short)ack[2] << 8) + (unsigned short)ack[3];
//        //qDebug() << "block=" << block;
//        if((block >= base && block < nextseqnum) ||
//                (nextseqnum < base && (block >= base || block < nextseqnum))){
//            base = block + 1;
//        }
//        else continue;
//        unsigned short y = base + N;
//        while(nextseqnum != y && packet[(nextseqnum - 1 + N) % N].dataLen == TFTP_NOT_LAST_DATA_LEN){
//            packet[nextseqnum % N].dataLen = file.read(data, TFTP_NOT_LAST_DATA_LEN);
//            packet[nextseqnum % N].data = makeTftpData(data, packet[nextseqnum % N].dataLen, nextseqnum);
//            uSock->write(packet[nextseqnum % N].data.data(), packet[nextseqnum % N].data.size());
//            ++nextseqnum;
//        }
//        if(base == nextseqnum) break;
//        //qDebug() << "base =" << base << " nextseqnum = " << nextseqnum;
//        retrans_times = 0;
//    }
//    file.close();
//    if(*mainThreadExitedOrNot){
//        *errorMessage = QString("主线程已退出");
//        return false;
//    }
//    if(retrans_times == max_retrans_times){
//        *errorMessage = QString("等待%1:ACK数据包超时").arg(fileName);
//        return false;
//    }
//    return true;
//}



//bool Tftp::receiveFile(QUdpSocket *uSock, QString path, QString *errorMessage, bool *mainThreadExitedOrNot, TftpMode tftpMode)
//{
//    QFile file(path);
//    QString fileName = path.lastIndexOf('/') != -1 ? path.mid(path.lastIndexOf('/') + 1)
//                                                                  : path;
//    if(!file.open(QIODevice::WriteOnly)){
//        *errorMessage = QString("文件%1:打开失败").arg(fileName);
//        return false;
//    }
//    int dataLen = TFTP_NOT_LAST_DATA_LEN;
//    if(tftpMode == RRQ){
//        //此处为加载端发起TFTP读请求
//        unsigned int retrans_times = 0;
//        QByteArray rrq = makeTftpReadRequest(fileName);
//        bool readyRead = false;
//        do{
//            uSock->writeDatagram(rrq, QHostAddress("169.254.5.122"), 69);
//            //uSock->write(rrq.data(), rrq.size());
//            retrans_times++;
//            //uSock->wa
//            readyRead = uSock->waitForReadyRead(wait_time_ms);
//        }while(!(*mainThreadExitedOrNot) && !readyRead && retrans_times < max_retrans_times);
//        if(*mainThreadExitedOrNot){
//            *errorMessage = QString("主线程已退出");
//            return false;
//        }
//        if(!readyRead){
//            *errorMessage = QString("等待文件%1:DATA报文超时1").arg(fileName);
//            return false;
//        }
//    }
//    else{
//        //此处为设备端发起TFTP写请求
//        unsigned int retrans_times = 0;
//        bool readyRead = false;
//        do{
//            QByteArray ack = makeTftpAck(0);
//            //uSock->write(ack.data(), ack.size());
//            uSock->writeDatagram(ack, QHostAddress("169.254.5.122"), 69);
//            retrans_times++;
//            readyRead = uSock->waitForReadyRead(wait_time_ms);
//        }while(!(*mainThreadExitedOrNot) && !readyRead && retrans_times < max_retrans_times);
//        if(*mainThreadExitedOrNot){
//            *errorMessage = QString("主线程已退出");
//            return false;
//        }
//        if(!readyRead){
//            *errorMessage = QString("等待文件%1:DATA报文超时2").arg(fileName);
//            return false;
//        }
//    }
//    QByteArray ack;
//    QByteArray data;
//    quint16 expectedBlock = 1, block = -1;
//    bool readReady = false;

//    while(dataLen == TFTP_NOT_LAST_DATA_LEN){
//        unsigned int retrans_times = 0;
//        do{
//            readReady = uSock->waitForReadyRead(wait_time_ms);
//            if(readReady){
//                if(uSock->pendingDatagramSize() < 4){
//                    *errorMessage = QString("文件%1:DATA报文格式有误").arg(fileName);
//                    return false;
//                }
//                //uSock->bytesAvailable()
//                data.resize(uSock->pendingDatagramSize());
//                //qDebug() << uSock->pendingDatagramSize() << uSock->bytesAvailable();
//                uSock->readDatagram(data.data(), uSock->pendingDatagramSize(), nullptr, nullptr);
//                //uSock->read(data.data(), data.size());
//                quint16 high = (unsigned char)data.at(2);
//                quint16 low = (unsigned char)data.at(3);
//                block = (high << 8) + low;
//                if(block == expectedBlock){
//                    ack = makeTftpAck(expectedBlock);
//                    dataLen = data.size() - 4;
//                    if(dataLen){
//                        file.write(data.mid(4));
//                    }
//                }
//            }
//            retrans_times++;
//            uSock->writeDatagram(ack, QHostAddress("169.254.5.122"), 69);
//        }while(!(*mainThreadExitedOrNot) && (!readReady || block != expectedBlock) && retrans_times < max_retrans_times);
//        if(*mainThreadExitedOrNot){
//            *errorMessage = QString("主线程已退出");
//            return false;
//        }
//        if(!readReady || block != expectedBlock){
//            *errorMessage = QString("等待文件%1DATA报文超时3 block=%2, expectedBlock=%3").arg(fileName).arg(block).arg(expectedBlock);
//            return false;
//        }
//        ++expectedBlock;
//    }
//    file.close();
//    return true;
//}
