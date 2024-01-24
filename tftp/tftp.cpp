#include "tftp.h"
#include <algorithm>
//extern const unsigned short blksize_default;
//extern const unsigned short timeout_default;
extern unsigned short blksize_default;
extern unsigned short timeout_default;
extern unsigned short maxRetransmit_default;
//unsigned short Tftp::blksize = blksize_default;
//unsigned short Tftp::timeout = timeout_default;

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

Tftp::TftpPacketType Tftp::getTftpPacketType(const QByteArray &tftpPacket)
{
    if(tftpPacket.size() > 2 && tftpPacket[1] <= 6){
        return static_cast<TftpPacketType>(tftpPacket[1] - 1);
    }
    return UNDEFINED;
}

bool Tftp::checkBlockNo(const QByteArray &tftpPacket, unsigned short No)
{
    if(getTftpPacketType(tftpPacket) != DATA ||
            tftpPacket.size() < 4){
        return false;
    }
    unsigned short high = static_cast<unsigned char>(tftpPacket[2]);
    unsigned short low = static_cast<unsigned char>(tftpPacket[3]);
    unsigned short blockNo = (high << 8) + low;
    return blockNo == No;
}

bool Tftp::checkAckNo(const QByteArray& tftpPacket, unsigned short No){
    if(getTftpPacketType(tftpPacket) != ACK ||
            tftpPacket.size() < 4){
        return false;
    }
    unsigned short high = static_cast<unsigned char>(tftpPacket[2]);
    unsigned short low = static_cast<unsigned char>(tftpPacket[3]);
    unsigned short blockNo = (high << 8) + low;
    return blockNo == No;
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
        oack.append('\0');
        oack.append(it->second.data());
        oack.append('\0');
    }
    return oack;
}


bool Tftp::put(QUdpSocket *uSock, QString path, QString fileName, QString *errorMessage, const QHostAddress &address, const quint16 port)
{
    QByteArray wrq = makeTftpWriteRequest(fileName);
    QByteArray oack, ack;
    unsigned short retrans_times = 0;
    unsigned short blksize = blksize_default, timeout = timeout_default, maxRetransmit = maxRetransmit_default;
    do{
        uSock->writeDatagram(wrq, address, port);
    }while(retrans_times++ < max_retrans_times &&
           (!uSock->waitForReadyRead(timeout_default * 1000)||
           uSock->pendingDatagramSize() <= 0 ||
           uSock->readDatagram(oack.data(), uSock->pendingDatagramSize()) <= 0||
           getTftpPacketType(oack) != OACK));
    if(retrans_times >= max_retrans_times){
        *errorMessage = QString("等待OACK报文超时");
        return false;
    }

    //解析接收到的oack报文
    auto index = oack.indexOf("blksize");
    if(index != -1){
        index += sizeof("blksize");
        blksize = 0;
        while(static_cast<char>(oack[index]) != '\0'){
            blksize = blksize * 10 + static_cast<char>(oack[index]) - '0';
            ++index;
        }
    }
    index = oack.indexOf("timeout");
    if(index != -1){
        index+=sizeof ("timeout");
        timeout = 0;
        while(static_cast<char>(oack[index]) != '\0'){
            timeout = timeout * 10 + static_cast<char>(oack[index]) - '0';
            ++index;
        }
    }
    index = oack.indexOf("max-retransmit");
    if(index != -1){
        index += sizeof("max-retransmit");
        maxRetransmit = 0;
        while(static_cast<char>(oack[index]) != '\0'){
            maxRetransmit = maxRetransmit * 10 + static_cast<char>(oack[index]) - '0';
            ++index;
        }
    }

    //后续的data报文和ack报文交互环节
    return upload(uSock, path, fileName, errorMessage, address, port, blksize, timeout, maxRetransmit);
}


bool Tftp::get(QUdpSocket *uSock, QString path, QString fileName, QString *errorMessage, const QHostAddress& host, const quint16 port)
{
    QByteArray readRequest = makeTftpReadRequest(fileName);
    QByteArray response, ack;
    unsigned short blksize = blksize_default, timeout = timeout_default, maxRetransmit = maxRetransmit_default;
    response.resize(blksize + 4);
    unsigned short retransTimes = 0;
    //1.consultation stage
    //1.1 send RRQ with options
    do{
        uSock->writeDatagram(readRequest, host, port);
    }while(retransTimes++ < max_retrans_times &&
           (!uSock->waitForReadyRead(timeout * 1000) ||
            uSock->pendingDatagramSize() <= 0 ||
            uSock->readDatagram(response.data(), uSock->pendingDatagramSize()) <= 0 ||
            getTftpPacketType(response) != OACK));
    qDebug() << retransTimes << max_retrans_times;
    if(retransTimes >= max_retrans_times){
        *errorMessage = QString("等待OACK报文超时");
        return false;
    }

    //1.2 receive and parse OACK
    auto index = response.indexOf("blksize");
    if(index != -1){
        index += sizeof("blksize");
        blksize = 0;
        while(static_cast<char>(response[index]) != '\0'){
            blksize = blksize * 10 + static_cast<char>(response[index]) - '0';
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
    index = response.indexOf("max-retransmit");
    if(index != -1){
        index += sizeof("max-retransmit");
        maxRetransmit = 0;
        while(static_cast<char>(response[index]) != '\0'){
            maxRetransmit = maxRetransmit * 10 + static_cast<char>(response[index]) - '0';
            ++index;
        }

    }

    //1.3 send ACK 0
    ack = makeTftpAck(0);
    uSock->writeDatagram(ack, host, port);



    //2.data transfer stage
    //uSock->readAll();

    return download(uSock, path, fileName, errorMessage, host, port, blksize, timeout, maxRetransmit, ack);

}

bool Tftp::handlePut(QUdpSocket *uSock, QString path, QString fileName, QString *errorMessage, const QHostAddress &host, const quint16 port, QByteArray writeRequest)
{
    //1.consulting stage
    unsigned short blksize = blksize_default, timeout = timeout_default,maxRetransmit = maxRetransmit_default;
    unsigned short blkSizeFromRequest = -1, timeoutFromRequest = -1, maxRetransmitFromRequest = -1;

    auto index = writeRequest.indexOf("blksize");
    if(index != -1){
        index += sizeof("blksize");
        blkSizeFromRequest = 0;
        while(static_cast<char>(writeRequest[index]) != '\0'){
            blkSizeFromRequest = blkSizeFromRequest * 10 + static_cast<char>(writeRequest[index]) - '0';
            ++index;
        }
        //如果blkSizeFromRequest在范围内
        if(blkSizeFromRequest >= 512 && blkSizeFromRequest <= 16 * 1024){
            blksize = std::min(blksize, static_cast<unsigned short>(blkSizeFromRequest));
        }
    }
    index = writeRequest.indexOf("timeout");
    if(index != -1){
        index += sizeof("timeout");
        timeoutFromRequest = 0;
        while(static_cast<char>(writeRequest[index]) != '\0'){
            timeoutFromRequest = timeoutFromRequest * 10 + static_cast<char>(writeRequest[index]) - '0';
            ++index;
        }
        //如果timeoutFromRequest在范围内, time
        timeout = std::min(timeout, static_cast<unsigned short>(timeoutFromRequest));
    }
    index = writeRequest.indexOf("max-retransmit");
    if(index != -1){
        index += sizeof ("max-retransmit");
        maxRetransmitFromRequest = 0;
        while(static_cast<char>(writeRequest[index]) != '\0'){
            maxRetransmitFromRequest = maxRetransmitFromRequest * 10 + static_cast<char>(writeRequest[index]) - '0';
        }
        //TODO
        maxRetransmit = std::min(maxRetransmit, static_cast<unsigned short>(maxRetransmitFromRequest));
    }
    QByteArray oAck = makeTftpOAck({std::make_pair(std::string("blksize"), std::to_string(blksize)),
                                   std::make_pair(std::string("timeout"), std::to_string(timeout)),
                                   std::make_pair(std::string("max-retransmit"), std::to_string(maxRetransmit))
                                   });

    uSock->writeDatagram(oAck, host, port);
    return download(uSock, path, fileName, errorMessage, host, port, blksize, timeout, maxRetransmit, oAck);
}

bool Tftp::handleGet(QUdpSocket *uSock, QString path, QString fileName, QString *errorMessage, const QHostAddress &host, const quint16 port, QByteArray readRequest)
{
    auto index = readRequest.indexOf("blksize");
    unsigned short blksize = blksize_default, timeout = timeout_default,maxRetransmit = maxRetransmit_default;
    unsigned short blkSizeFromRequest = -1, timeoutFromRequest = -1, maxRetransmitFromRequest = -1;
    if(index != -1){
        index += sizeof("blksize");
        blkSizeFromRequest = 0;
        while(static_cast<char>(readRequest[index]) != '\0'){
            blkSizeFromRequest = blkSizeFromRequest * 10 + static_cast<char>(readRequest[index]) - '0';
            ++index;
        }
        //如果blkSizeFromRequest在范围内, 则取它和blksize的最小值
        if(blkSizeFromRequest >= 512 && blksize <= 16 * 1024){
            blksize = std::min(blksize, static_cast<unsigned short>(blkSizeFromRequest));
        }
    }
    index = readRequest.indexOf("timeout");
    if(index != -1){
        index += sizeof("timeout");
        timeoutFromRequest = 0;
        while(static_cast<char>(readRequest[index]) != '\0'){
            timeoutFromRequest = timeoutFromRequest * 10 + static_cast<char>(readRequest[index]) - '0';
            ++index;
        }
        //如果timeoutFromRequest在范围内, 则取它和timeout的最小值
        timeout = std::min(timeout, static_cast<unsigned short>(timeoutFromRequest));
    }

    index = readRequest.indexOf("max-retransmit");
    if(index != -1){
        index += sizeof ("max-retransmit");
        maxRetransmitFromRequest = 0;
        while(static_cast<char>(readRequest[index]) != '\0'){
            maxRetransmitFromRequest = maxRetransmitFromRequest * 10 + static_cast<char>(readRequest[index]) - '0';
        }
        //TODO
        maxRetransmit = std::min(maxRetransmit, static_cast<unsigned short>(maxRetransmitFromRequest));
    }
    QByteArray oack = makeTftpOAck({std::make_pair(std::string("blkSize"), std::to_string(blksize)),
                                   std::make_pair(std::string("timeout"), std::to_string(timeout)),
                                   std::make_pair(std::string("max-retransmit"), std::to_string(maxRetransmit))});
    uSock->writeDatagram(oack, host, port);
    return upload(uSock, path, fileName, errorMessage, host, port, blksize, timeout, maxRetransmit);
}


bool Tftp::download(QUdpSocket *uSock, QString path, QString fileName, QString *errorMessage, const QHostAddress &host, const quint16 port, unsigned short blksize, unsigned short timeout, unsigned short maxRetransmit, QByteArray& previousPacket)
{
    QFile file(path + "/" + fileName);
    if(file.open(QIODevice::WriteOnly) == false){
        *errorMessage = QString("文件打开失败%1").arg(fileName);
        return false;
    }
    //QByteArray previousPacekt = ;
    QByteArray dataPacket;
    dataPacket.resize(blksize + 4);
    unsigned short transTimes = 0;
    unsigned short dataLen = 0;
    unsigned short expectedBlockNo = 1;
//
//    if((!uSock->waitForReadyRead(timeout * 1000) ||
//           uSock->pendingDatagramSize() <= 0 ||
//           uSock->readDatagram(dataPacket.data(), uSock->pendingDatagramSize()) <= 0 ||
//           !checkBlockNo(dataPacket, expectedBlockNo))){
//        *errorMessage = QString("等待块号为1的DATA报文超时");
//        return false;
//    }
//
//    file.write(dataPacket.mid(4));
//    QByteArray ack = makeTftpAck(expectedBlockNo++);
//    uSock->writeDatagram(ack, host, port);
//    previousPacket = ack;
    do{
        while(transTimes++ < maxRetransmit &&
              (!uSock->waitForReadyRead(timeout * 1000) ||
               uSock->pendingDatagramSize() <= 0 ||
               uSock->readDatagram(dataPacket.data(), uSock->pendingDatagramSize()) <= 0 ||
               !checkBlockNo(dataPacket, expectedBlockNo))){
            uSock->writeDatagram(previousPacket, host, port);
        }
        if(transTimes >= maxRetransmit){
            *errorMessage = QString("等待DATA报文超时");
            return false;
        }
        file.write(dataPacket.mid(4));
        QByteArray ack = makeTftpAck(expectedBlockNo++);
        uSock->writeDatagram(ack, host, port);
        previousPacket = ack;
    }while(dataLen == blksize);
    file.close();
    return true;
}

bool Tftp::upload(QUdpSocket *uSock, QString path, QString fileName, QString *errorMessage, const QHostAddress &host, const quint16 port, unsigned short blksize, unsigned short timeout, unsigned short maxRetransmit)
{
     QByteArray previousPacket;
     QByteArray ack;
     char* data = new char[blksize];
     unsigned short retrans_times = 0;
     unsigned short dataLen = 0;
     unsigned short BlockNo = 1;
     QFile file(path + '/' +fileName);
     if(!file.open(QIODevice::WriteOnly)){
        *errorMessage = QString("文件%1:打开失败").arg(fileName);
        return false;
     }
     do{
         dataLen = file.read(data,blksize);
         QByteArray dataWaitforTransferPacket = makeTftpData(data, sizeof(data), BlockNo);
         uSock->writeDatagram(dataWaitforTransferPacket, host, port);
         previousPacket = dataWaitforTransferPacket;
         while(retrans_times < maxRetransmit &&
               (!uSock->waitForReadyRead(timeout * 1000) ||
                uSock->pendingDatagramSize() <= 0 ||
                uSock->readDatagram(ack.data(), uSock->pendingDatagramSize()) <=0 ||
               !checkBlockNo(ack, BlockNo))){
             uSock->writeDatagram(previousPacket, host, port);
         }
         if(retrans_times >= maxRetransmit){//报错
             *errorMessage = QString("等待ACK报文超时");
             return false;
         }
     }while(dataLen == blksize);
     file.close();
     delete[] data;
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


