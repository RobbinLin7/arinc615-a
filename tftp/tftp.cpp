#include "tftp.h"
#include <algorithm>
//extern const unsigned short blksize_default;
//extern const unsigned short timeout_default;
extern unsigned short blksize_default;
extern unsigned short timeout_default;
extern unsigned short maxRetransmit_default;
//unsigned short Tftp::blksize = blksize_default;
//unsigned short Tftp::timeout = timeout_default;

QByteArray Tftp::makeTftpReadRequest(QString fileName, quint16 valueOfBlockSize, quint16 valueOfTimeOut, quint16 maxRetransmit, QString mode)
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
    //6.max-retransmit
    request.append("max-retransmit");
    request.append('\0');
    request.append(QString::number(maxRetransmit));
    request.append('\0');
    return request;
}


QByteArray Tftp::makeTftpWriteRequest(QString fileName, quint16 valueOfBlockSize, quint16 valueOfTimeOut, quint16 maxRetransmit, QString mode)
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
    request.append(QString::number(valueOfBlockSize));
    request.append('\0');

    //5.timeout 范围为1-65535

    request.append("timeout");
    request.append('\0');
    request.append(QString::number(valueOfTimeOut));
    request.append('\0');

    //6.max-retransmit

    request.append("max-retransmit");
    request.append('\0');
    request.append(QString::number(maxRetransmit));
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

QByteArray Tftp::makeTftpAbort(unsigned short statusCode)
{
    return makeTftpError(0, QString("ABORT:%1").arg(statusCode, 4, 16, QChar('0')));
}

Tftp::TftpPacketType Tftp::getTftpPacketType(const QByteArray &tftpPacket)
{
    if(tftpPacket.size() > 2 && tftpPacket[1] <= 6){
        return static_cast<TftpPacketType>(tftpPacket[1]);
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
    unsigned short ackNo = (high << 8) + low;
    return ackNo == No;
}

unsigned short Tftp::getNo(const QByteArray &tftpPacket)
{
    unsigned short high = static_cast<unsigned char>(tftpPacket[2]);
    unsigned short low = static_cast<unsigned char>(tftpPacket[3]);
    unsigned short no = (high << 8) + low;
    return no;
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


bool Tftp::put(QUdpSocket *uSock, QString path, QString fileName, QString *errorMessage, const QHostAddress &address, quint16 port)
{
    QByteArray wrq = makeTftpWriteRequest(fileName, blksize_default, timeout_default, maxRetransmit_default);
    QByteArray oack, ack;
    quint16 newPort;
    unsigned short retrans_times = 0;
    unsigned short blksize = blksize_default, timeout = timeout_default, maxRetransmit = maxRetransmit_default;
    oack.resize(blksize + 4);
    do{
        uSock->writeDatagram(wrq, address, port);
    }while(retrans_times++ < maxRetransmit &&
           (!uSock->waitForReadyRead(timeout_default * 1000)||
           uSock->pendingDatagramSize() <= 0 ||
           uSock->readDatagram(oack.data(), uSock->pendingDatagramSize(), nullptr, &newPort) <= 0||
           getTftpPacketType(oack) != OACK));
    if(retrans_times >= maxRetransmit){
        *errorMessage = QString("等待OACK报文超时");
        return false;
    }

    //解析接收到的oack报文
    port = newPort;
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


bool Tftp::get(QUdpSocket *uSock, QString path, QString fileName, QString *errorMessage, const QHostAddress& host, quint16 port)
{
    QByteArray readRequest = makeTftpReadRequest(fileName, blksize_default, timeout_default, maxRetransmit_default);
    QByteArray response, ack;
    quint16 newPort;
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
            uSock->readDatagram(response.data(), uSock->pendingDatagramSize(), nullptr, &newPort) <= 0 ||
            getTftpPacketType(response) != OACK));
    qDebug() << retransTimes << max_retrans_times;
    if(retransTimes >= max_retrans_times){
        *errorMessage = QString("等待OACK报文超时");
        return false;
    }

    //1.2 receive and parse OACK
    port = newPort;
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
    //qDebug() << "port = " << port << "in handlePut";
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
            ++index;
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
            ++index;
        }
        //TODO
        maxRetransmit = std::min(maxRetransmit, static_cast<unsigned short>(maxRetransmitFromRequest));
    }
    QByteArray oack = makeTftpOAck({std::make_pair(std::string("blksize"), std::to_string(blksize)),
                                   std::make_pair(std::string("timeout"), std::to_string(timeout)),
                                   std::make_pair(std::string("max-retransmit"), std::to_string(maxRetransmit))});
    uSock->writeDatagram(oack, host, port);
    unsigned short retry = 0;
    //wait for Ack 0
    while(1){
        while(uSock->waitForReadyRead(1000 * timeout) == false && retry < maxRetransmit){
            uSock->writeDatagram(oack, host, port);
            ++retry;
        }
        if(uSock->hasPendingDatagrams() == false){
            *errorMessage = QString("等待ACK 0超时");
            return false;
        }
        QByteArray ack;
        ack.resize(uSock->pendingDatagramSize());
        uSock->readDatagram(ack.data(), ack.size());
        if(getTftpPacketType(ack) != ACK){
            qDebug() << "期待ACK报文 但收到OPCODE=" << getTftpPacketType(ack);
            if(retry < maxRetransmit){
                uSock->writeDatagram(oack, host, port);
                ++retry;
                continue;
            }
            else{
                *errorMessage = QString("等待ACK 0超时");
                return false;
            }
        }
        else{
            if(getNo(ack) != 0){
                qDebug() << QString("期待收到ACK %1 但是收到%2").arg(0).arg(getNo(ack));
                if(retry < maxRetransmit){
                    uSock->writeDatagram(oack, host, port);
                    ++retry;
                    continue;
                }
                else{
                    *errorMessage = QString("等待ACK 0超时");
                    return false;
                }

            }
            else break;
        }
    }
    qDebug() << "receive ack 0 success in handleGet()";
//    QByteArray ack;
//    ack.resize(uSock->pendingDatagramSize());
//    uSock->readDatagram(ack.data(), ack.size());
    return upload(uSock, path, fileName, errorMessage, host, port, blksize, timeout, maxRetransmit);
}


bool Tftp::download(QUdpSocket *uSock, QString path, QString fileName, QString *errorMessage, const QHostAddress &host, const quint16 port, unsigned short blksize, unsigned short timeout, unsigned short maxRetransmit, QByteArray& previousPacket)
{
    //qDebug() << "port = " << port;
    QFile file(path + "/" + fileName);
    if(file.open(QIODevice::WriteOnly) == false){
        *errorMessage = QString("文件打开失败%1").arg(fileName);
        return false;
    }
    QByteArray dataPacket;
    dataPacket.resize(blksize + 4);
    unsigned short transTimes = 1;
    unsigned short dataLen = 0;
    unsigned short expectedBlockNo = 1;
    do{
        while(transTimes < maxRetransmit + 1 &&
              (!uSock->waitForReadyRead(timeout * 1000) ||
               (dataLen = uSock->pendingDatagramSize()) <= 0 ||
               uSock->readDatagram(dataPacket.data(), uSock->pendingDatagramSize()) <= 0 ||
               !checkBlockNo(dataPacket, expectedBlockNo))){
            uSock->writeDatagram(previousPacket, host, port);
            ++transTimes;
        }
        if(transTimes >= maxRetransmit + 1){
            *errorMessage = QString("等待DATA报文超时");
            return false;
        }
        file.write(dataPacket.mid(4), dataLen - 4);
        QByteArray ack = makeTftpAck(expectedBlockNo++);
        qDebug() << host << " " << port;
        uSock->writeDatagram(ack, host, port);
        transTimes = 1;
        previousPacket = ack;
    }while(dataLen - 4 == blksize);
    file.close();
    return true;
}

bool Tftp::upload(QUdpSocket *uSock, QString path, QString fileName, QString *errorMessage, const QHostAddress &host, const quint16 port, unsigned short blksize, unsigned short timeout, unsigned short maxRetransmit)
{
     QByteArray previousPacket;
     QByteArray ack;
     ack.resize(4);
     char* data = new char[blksize + 4];

     unsigned short trans_times = 0;
     unsigned short dataLen = 0;
     unsigned short blockNo = 1;
     QFile file(path + '/' +fileName);
     if(!file.open(QIODevice::ReadOnly)){
        *errorMessage = QString("文件%1:打开失败").arg(fileName);
        return false;
     }
     do{
         dataLen = file.read(data + 4, blksize);
         makeTftpData(data, blockNo);
         //QByteArray dataWaitforTransferPacket = makeTftpData(data, dataLen, blockNo);
         uSock->writeDatagram(data, dataLen + 4, host, port);
         //uSock->writeDatagram(data, host, port);
         trans_times = 1;
         //previousPacket = dataWaitforTransferPacket;
         while(1){
             while(!uSock->waitForReadyRead(timeout * 1000) && trans_times < maxRetransmit + 1){
                 //uSock->writeDatagram(previousPacket, host, port);
                 uSock->writeDatagram(data, dataLen + 4, host, port);
                 ++trans_times;
             }
             if(uSock->pendingDatagramSize() <= 0 && trans_times >= maxRetransmit + 1){
                 *errorMessage = QString("等待ACK报文超时");
                 return false;
             }
             while(uSock->pendingDatagramSize() > 0){
                 uSock->readDatagram(ack.data(), uSock->pendingDatagramSize());
             }
             if(checkAckNo(ack, blockNo)) break;
             //uSock->writeDatagram(previousPacket, host, port);
             uSock->writeDatagram(data, dataLen + 4, host, port);
             ++trans_times;
         }

//         while(
//               (!uSock->waitForReadyRead(timeout * 1000) ||
//                uSock->pendingDatagramSize() <= 0 ||
//                uSock->readDatagram(ack.data(), uSock->pendingDatagramSize()) <=0 ||
//               !checkAckNo(ack, blockNo))
//               && trans_times < maxRetransmit + 1){
//             uSock->writeDatagram(previousPacket, host, port);
//             trans_times++;
//         }
//         if(trans_times >= maxRetransmit + 1){//报错
//             *errorMessage = QString("等待ACK报文超时");
//             return false;
//         }
         ++blockNo;
         //qDebug() << "dataLen = " << dataLen << "blksize = " << blksize;
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

char *Tftp::makeTftpData(char data[], quint16 block)
{
    data[0] = '\0';
    data[1] = 3;

    data[2] = block >> 8;
    data[3] = block;

    return data;
}


