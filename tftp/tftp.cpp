#include "tftp.h"

//bool Tftp::sendFile(QUdpSocket *uSock, QString path, QString* errorMessage, enum TftpMode tftpMode)
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
//        }while(!readyRead && retrans_times < max_retrans_times);
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
//    //如果设备端接受了写请求
//    do{
//        QByteArray tftpData, ack;
//        unsigned int retrans_times = 0;
//        dataLen = file.read(data, TFTP_NOT_LAST_DATA_LEN);
//        tftpData = makeTftpData(data, dataLen, block);
//        bool readyRead = false;
//        quint16 ackNo = 0;
//        //当readReady为True并且接收到正确编号的ACK报文后或者重传次数超出后跳出
//        while((!readyRead || block != ackNo) && retrans_times <= max_retrans_times){
//            uSock->write(tftpData.data(), tftpData.size());
//            readyRead = uSock->waitForReadyRead(wait_time_ms);//设置3秒的超时时间
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
//        if(retrans_times > max_retrans_times){
//            *errorMessage = QString("等待%1:ACK数据包超时").arg(fileName);
//            return false;
//        }
//        block++;
//    }while(dataLen == TFTP_NOT_LAST_DATA_LEN);//直到组成一个512字节的data包
//    file.close();
//    return true;
//}

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

//bool Tftp::sendFile(QHostAddress targetAddress, QUdpSocket *uSock, QString path, QString *errorMessage, bool *mainThreadExitedOrNot, TftpMode tftpMode)
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


//bool Tftp::receiveFile(QUdpSocket *uSock, QString path, QString* errorMessage, enum TftpMode tftpMode)
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
//            uSock->write(rrq.data(), rrq.size());
//            retrans_times++;
//            readyRead = uSock->waitForReadyRead(wait_time_ms);
//        }while(!readyRead && retrans_times < max_retrans_times);
//        if(!readyRead){
//            *errorMessage = QString("等待文件%1:DATA报文超时").arg(fileName);
//            return false;
//        }
//    }
//    else{
//        //此处为设备端发起TFTP写请求
//        unsigned int retrans_times = 0;
//        bool readyRead = false;
//        do{
//            QByteArray ack = makeTftpAck(0);
//            uSock->write(ack.data(), ack.size());
//            retrans_times++;
//            readyRead = uSock->waitForReadyRead(wait_time_ms);
//        }while(!readyRead && retrans_times < max_retrans_times);
//        if(!readyRead){
//            *errorMessage = QString("等待文件%1:DATA报文超时").arg(fileName);
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
//            readReady = uSock->waitForReadyRead();
//            if(readReady){
//                if(uSock->pendingDatagramSize() < 4){
//                    *errorMessage = QString("文件%1:DATA报文格式有误").arg(fileName);
//                    return false;
//                }
//                //uSock->bytesAvailable()
//                data.resize(uSock->pendingDatagramSize());
//                //qDebug() << uSock->pendingDatagramSize() << uSock->bytesAvailable();
//                uSock->read(data.data(), data.size());
//                quint16 high = (unsigned char)data.at(2);
//                quint16 low = (unsigned char)data.at(3);
//                block = (high << 8) + low;
//                if(block == expectedBlock){
//                    ack = makeTftpAck(expectedBlock);
//                    dataLen = data.size() - 4;
////                    if(dataLen){

////                    }
//                    if(dataLen){
//                        file.write(data.mid(4));
//                    }
//                }
//            }
//            retrans_times++;
//            uSock->write(ack.data(), ack.size());
//        }while((!readReady || block != expectedBlock) && retrans_times < max_retrans_times);
//        if(!readReady || block != expectedBlock){
//            *errorMessage = QString("等待文件%1DATA报文超时");
//            return false;
//        }
//        ++expectedBlock;
//    }
//    file.close();
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
//    quint16 expectedBlock = 0, block = -1;

//    if(tftpMode == RRQ){
//        //此处为加载端发起TFTP读请求
//        unsigned int retrans_times = 0;
//        QByteArray rrq = makeTftpReadRequest(fileName);
//        bool readyRead = false;
//        do{
//            uSock->write(rrq.data(), rrq.size());
//            retrans_times++;
//            //uSock->wa
//            readyRead = uSock->waitForReadyRead(wait_time_ms);
//        }while(!(*mainThreadExitedOrNot) && !readyRead && retrans_times < max_retrans_times);
//        if(*mainThreadExitedOrNot){
//            *errorMessage = QString("主线程已退出");
//            return false;
//        }
//        if(!readyRead){
//            *errorMessage = QString("等待文件%1:OACK报文超时").arg(fileName);
//            return false;
//        }
//        //如果主程序没有退出并且读到了OACK报文 先读取Oack包并且返回ACK包
//        if(*mainThreadExitedOrNot && readyRead){
//            QByteArray oAck,ackReplyToOack;
//            bool readyRead = false;
//            oAck.resize(uSock->pendingDatagramSize());
//            uSock->read(oAck.data(),oAck.size());
//            //解析OACK报文中的字段

//            //构造回复OACK报文的ack包
//            do{
//                ackReplyToOack = makeTftpAck(expectedBlock);//块号从0开始
//                uSock->write(ackReplyToOack.data(), ackReplyToOack.size());
//                retrans_times++;
//                readyRead = uSock->waitForReadyRead(wait_time_ms);
//            }while(!(*mainThreadExitedOrNot) && !readyRead && retrans_times < max_retrans_times);
//            if(*mainThreadExitedOrNot){
//                *errorMessage = QString("主线程已退出");
//                return false;
//            }
//            if(!readyRead){
//                *errorMessage = QString("等待文件%1:DATA报文超时").arg(fileName);
//                return false;
//            }
//        }

//    }
//    else{
//        //此处为设备端发起TFTP写请求
//        unsigned int retrans_times = 0;
//        bool readyRead = false;
//        do{
//            QByteArray ack = makeTftpAck(0);
//            uSock->write(ack.data(), ack.size());
//            retrans_times++;
//            readyRead = uSock->waitForReadyRead(wait_time_ms);
//        }while(!(*mainThreadExitedOrNot) && !readyRead && retrans_times < max_retrans_times);
//        if(*mainThreadExitedOrNot){
//            *errorMessage = QString("主线程已退出");
//            return false;
//        }
//        if(!readyRead){
//            *errorMessage = QString("等待文件%1:DATA报文超时").arg(fileName);
//            return false;
//        }
//    }
//    QByteArray ack;
//    QByteArray data;
////    quint16 expectedBlock = 1, block = -1;
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
//                uSock->read(data.data(), data.size());
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
//            uSock->write(ack.data(), ack.size());
//        }while(!(*mainThreadExitedOrNot) && (!readReady || block != expectedBlock) && retrans_times < max_retrans_times);
//        if(*mainThreadExitedOrNot){
//            *errorMessage = QString("主线程已退出");
//            return false;
//        }
//        if(!readReady || block != expectedBlock){
//            *errorMessage = QString("等待文件%1DATA报文超时").arg(fileName);
//            return false;
//        }
//        ++expectedBlock;
//    }
//    file.close();
//    return true;
//}

//bool Tftp::receiveFile(QHostAddress address, QUdpSocket *uSock, QString path, QString *errorMessage, bool *mainThreadExitedOrNot, TftpMode tftpMode)
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
//            //uSock->writeDatagram(rrq, )
//            uSock->write(rrq.data(), rrq.size());
//            uSock->bind(uSock->localPort());
//            //uSock->bind();

//            retrans_times++;
//            //uSock->wa
//            readyRead = uSock->waitForReadyRead(wait_time_ms);
//        }while(!(*mainThreadExitedOrNot) && !readyRead && retrans_times < max_retrans_times);
//        if(*mainThreadExitedOrNot){
//            *errorMessage = QString("主线程已退出");
//            return false;
//        }
//        if(!readyRead){
//            *errorMessage = QString("等待文件%1:DATA报文超时").arg(fileName);
//            return false;
//        }

//    }
//    else{
//        //此处为设备端发起TFTP写请求
//        unsigned int retrans_times = 0;
//        bool readyRead = false;
//        do{
//            QByteArray ack = makeTftpAck(0);
//            uSock->write(ack.data(), ack.size());
//            retrans_times++;
//            readyRead = uSock->waitForReadyRead(wait_time_ms);
//        }while(!(*mainThreadExitedOrNot) && !readyRead && retrans_times < max_retrans_times);
//        if(*mainThreadExitedOrNot){
//            *errorMessage = QString("主线程已退出");
//            return false;
//        }
//        if(!readyRead){
//            *errorMessage = QString("等待文件%1:DATA报文超时").arg(fileName);
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
////            QByteArray datagram;
////            QString fileName;
////            datagram.resize(uSock->pendingDatagramSize());
////            QHostAddress address;
////            quint16 port;
////            uSock->readDatagram(datagram.data(), datagram.size(), &address, &port);
////            uSock->connectToHost(address, port);
////            qDebug() << "port" << port;
//            if(readReady){
//                if(uSock->pendingDatagramSize() < 4){
//                    *errorMessage = QString("文件%1:DATA报文格式有误").arg(fileName);
//                    return false;
//                }
//                //uSock->bytesAvailable()
//                data.resize(uSock->pendingDatagramSize());
//                //qDebug() << uSock->pendingDatagramSize() << uSock->bytesAvailable();
//                uSock->read(data.data(), data.size());
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
//            uSock->write(ack.data(), ack.size());
//        }while(!(*mainThreadExitedOrNot) && (!readReady || block != expectedBlock) && retrans_times < max_retrans_times);
//        if(*mainThreadExitedOrNot){
//            *errorMessage = QString("主线程已退出");
//            return false;
//        }
//        if(!readReady || block != expectedBlock){
//            *errorMessage = QString("等待文件%1DATA报文超时").arg(fileName);
//            return false;
//        }
//        ++expectedBlock;
//    }
//    file.close();
//    return true;
//}



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


//bool Tftp::put(QUdpSocket *uSock, QString path, QString fileName, QString *errorMessage)
//{


//    QFile file(path + '/' +fileName);
//       if(!file.open(QIODevice::ReadOnly)){
//           *errorMessage = QString("文件%1打开失败").arg(fileName);
//           return false;
//       }

////       int dataLen = -1;
////       quint16 block = 1;
////       char data[blkSize];
////       memset(data, 0, sizeof(data));
//       QByteArray wrq = makeTftpWriteRequest(fileName);
//       bool readyRead = false;

//       do{
//           uSock->write(wrq.data(), wrq.size());
//           retrans_times++;
//           readyRead = uSock->waitForReadyRead(wait_time_ms);
//       }while(!readyRead && retrans_times < max_retrans_times);

//       //如果一直没有接收到wrq的Oack包
//       if(!readyRead){
//           *errorMessage = QString("%1写请求相应超时").arg(fileName);
//           return false;
//       }else{
//           //先读取得到的oack包
//           if(uSock->pendingDatagramSize() < 2){
//                *errorMessage = QString("文件%1:OACK报文格式有误").arg(fileName);
//                return false;
//           }
//           QByteArray oAck;
//           oAck.resize(uSock->pendingDatagramSize());
//           uSock->read(oAck.data(),oAck.size());
//           parseOACK(oAck);//设置blksize和timeout的值
//       }
//       //之后的data报文和ack报文交互环节
//       do{

//           int dataLen = -1;
//           quint16 block = 1;
//           quint16 ackNo = 0;
//           unsigned int retrans_times = 0;
//           bool readyRead = false;
//           char data[blkSize];
//           memset(data, 0, sizeof(data));
//           QByteArray tftpData, ack;



//           dataLen = file.read(data, blkSize);
//           tftpData = makeTftpData(data, dataLen, block);
//           //当readReady为True并且接收到正确编号的ACK报文后或者重传次数超出后跳出
//           while((!readyRead || block != ackNo) && retrans_times <= max_retrans_times){
//               uSock->write(tftpData.data(), tftpData.size());
//               readyRead = uSock->waitForReadyRead(wait_time_ms);//设置3秒的超时时间
//               if(readyRead){
//                   if(uSock->pendingDatagramSize() < 4){
//                       *errorMessage = QString("%1:设备端端口不可达").arg(fileName);
//                       return false;
//                   }
//                   ack.resize(uSock->pendingDatagramSize());
//                   uSock->read(ack.data(), ack.size());
//                   quint16 high = (unsigned char)ack.at(2);
//                   quint16 low = (unsigned char)ack.at(3);
//                   ackNo = (high << 8) + low;
//               }
//               retrans_times++;
//           }
//           if(retrans_times > max_retrans_times){
//               *errorMessage = QString("等待%1:ACK数据包超时").arg(fileName);
//               return false;
//           }
//           block++;
//       }while(dataLen == TFTP_NOT_LAST_DATA_LEN);//直到组成一个512字节的data包
//       file.close();
//       return true;
//}


void parseOACK(QByteArray &oackData) {

    int index = 2;
    QString optionName, optionValue;
    while (index < oackData.size()) {
        optionName.clear();
        optionValue.clear();

        while (index < oackData.size() && oackData.at(index) != '\0') {
            optionName.append(oackData.at(index));
            index++;
        }
        index++;

        while (index < oackData.size() && oackData.at(index) != '\0') {
            optionValue.append(oackData.at(index));
            index++;
        }
        index++;


        if (optionName == "blksize") {
             blkSize = optionValue.toInt();
        } else if (optionName == "timeout") {
             timeOut = optionValue.toInt();
        }
    }

//        qDebug() << "Option Name: " << optionName << ", Option Value: " << optionValue;
}

bool Tftp::handleGet(QUdpSocket *uSock, QString path, QString fileName, QString *errorMessage)
{

}



bool Tftp::put(QUdpSocket *uSock, QString path, QString fileName, QString *errorMessage, const QHostAddress &address, const quint16 port)
{
    QByteArray wrq = makeTftpWriteRequest(fileName);
    QByteArray oack,ack;

    do{
        uSock->writeDatagram(wrq, address, port);
    }while(retrans_times++ < max_retrans_times &&
           !uSock->waitForReadyRead(timeout * 1000));
    //报错

    //接受oack报文

    //oack报文格式出错？
    if(uSock->pendingDatagramSize()<2){

    }
    oack.resize(uSock->pendingDatagramSize());
    uSock->readDatagram(oack.data(),oack.size());

    //解析接收到的oack报文
    auto index = oack.indexOf("blksize");
    if(index != -1){
        blkSize = 0;
        while(static_cast<char>(oack[index]) != '\0'){
            blkSize = blkSize * 10 + static_cast<char>(oack[index]) - '0';
            ++index;
        }
    }
    index = oack.indexOf("timeout");
    if(index != -1){
        timeout = 0;
        while(static_cast<char>(oack[index]) != '\0'){
            timeout = timeout * 10 + static_cast<char>(oack[index]) - '0';
            ++index;
        }
    }

    //构造并且发后续的ack报文
    ack = makeTftpAck(0);
    do{
        uSock->writeDatagram(ack, address, port);
    }while(retrans_times++ < max_retrans_times && !uSock->waitForReadyRead(timeout * 1000));
    if(retrans_times == max_retrans_times){//超时报错

    }

    //后续的data报文和ack报文交互环节
    upload(uSock, path, fileName, errorMessage, address, port, ack);


}

bool Tftp::get(QUdpSocket *uSock, QString path, QString fileName, QString *errorMessage, const QHostAddress& address, const quint16 port)
{
    QByteArray readRequest = makeTftpReadRequest(fileName);
    QByteArray response, ack;
    QFile file(path + "/" + fileName);
    unsigned short retransTimes = 0;
    //1.consultation stage

    //1.1 send RRQ with options
    do{
        uSock->writeDatagram(readRequest, address, port);
    }while(++retransTimes < max_retrans_times &&
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
        blkSize = 0;
        while(static_cast<char>(response[index]) != '\0'){
            blkSize = blkSize * 10 + static_cast<char>(response[index]) - '0';
            ++index;
        }
    }
    index = response.indexOf("timeout");
    if(index != -1){
        timeout = 0;
        while(static_cast<char>(response[index]) != '\0'){
            timeout = timeout * 10 + static_cast<char>(response[index]) - '0';
            ++index;
        }
    }

    //1.3 send ACK 0
    ack = makeTftpAck(0);
    uSock->writeDatagram(readRequest, address, port);



    //2.data transfer stage
    downLoad(uSock, path, fileName, errorMessage, address, port, ack);

}

bool Tftp::downLoad(QUdpSocket *uSock, QString path, QString fileName, QString *errorMessage, const QHostAddress &address, const quint16 port, QByteArray lastPacketOfConsult)
{
    QByteArray previousPacekt = lastPacketOfConsult;
    unsigned short retransTimes = 0;
    unsigned short dataLen = 0;
    //while(uSock)
    do{

    }while(dataLen == tftp)
}

bool Tftp::upload(QUdpSocket *uSock, QString path, QString fileName, QString *errorMessage, const QHostAddress &address, const quint16 port, QByteArray lastPacketOfConsult)
{
     QByteArray previousPacekt = lastPacketOfConsult;
     QFile file(path + '/' +fileName);
     if(!file.open(QIODevice::WriteOnly)){
        *errorMessage = QString("文件%1:打开失败").arg(fileName);
        return false;
     }

     while()








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
