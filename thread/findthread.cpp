#include "findthread.h"


void FindThread::run()
{
    QTimer timer;
    qDebug() << QThread::currentThreadId();
    QByteArray findRequest = makeFindRequest();
    uSock = new QUdpSocket(this);
    uSock->bind(QHostAddress::AnyIPv4, 0, QUdpSocket::ShareAddress);
    connect(uSock, &QUdpSocket::readyRead, this, &FindThread::onUsockReadReady);
    connect(&timer, &QTimer::timeout, this, &FindThread::onTimerTimeOut);
    uSock->writeDatagram(findRequest.data(), findRequest.size(), entry->broadcast(), 1001);
    timer.start(max_find_response_time_ms / 100);
    //开启事件循环
    exec();
}

QByteArray FindThread::makeFindRequest(){
    qDebug() << QThread::currentThreadId() << "makeFindRequest";
    find_req req;   //find请求
    memset(&req, 0, sizeof(req));
    req.opcode = 256;
    req.ASCII_String = '\0';
    req.Packet_Terminator = 0x10;
    req.tftpRetryNum = max_retrans_times;
    req.tftpTimeout = wait_time_ms;
    req.lusPeriod = state_file_send_interval;
    QByteArray datagram;
    appendBigEndian(&req.opcode, sizeof(req.opcode), datagram);
    datagram.append(req.ASCII_String).append(req.Packet_Terminator);
    appendBigEndian(&req.tftpRetryNum, sizeof(req.tftpRetryNum), datagram);
    appendBigEndian(&req.tftpTimeout, sizeof(req.tftpTimeout), datagram);
    appendBigEndian(&req.lusPeriod, sizeof(req.lusPeriod), datagram);
    return datagram;
}

void FindThread::appendBigEndian(void *arg, size_t size, QByteArray &byteArray)
{
    for(int i = size - 1; i > -1; i--){
        byteArray.append(*((char*)arg + i));
    }
}

void FindThread::onUsockReadReady()
{
    while(uSock->hasPendingDatagrams()){
        QByteArray findResponse;
        QHostAddress remoteAddress;
        //让datagrama的大小为等待处理的数据包的大小，这样才能接收到完整的数据
        findResponse.resize(uSock->pendingDatagramSize());
        //接收数据报，将其存放到datagram中
        uSock->readDatagram(findResponse.data(), findResponse.size(), &remoteAddress);
        findResponse = findResponse.trimmed();
        //去掉FIND相应报文的头部，获取内容
        if(findResponse.size() < 2 || strcmp(findResponse.left(2).data(), "\x02\x00")){
            return;
        }
        QString info = QString::fromStdString(findResponse.mid(2).toStdString());
        QStringList deviceInfo;
        //首先加上设备端的IP地址
        deviceInfo << remoteAddress.toString();
        //然后获取以0作为间隔的FIND相应内容
        deviceInfo << info.split('\0');
        //去掉所有的空字符串项和非字符项
        int i = 0;
        for(; i < deviceInfo.size(); i++){
            if(deviceInfo[i] == "") {
                deviceInfo.removeAt(i);
                i = -1;
            }
            else{
                for(int j = 0; j < deviceInfo[i].size(); j++){
                    if(deviceInfo[i][j] < 32 || deviceInfo[i][j] > 126){
                        deviceInfo.removeAt(i);
                        break;
                        i = -1;
                    }
                }
            }
        }
        emit(deviceFoundSignal(deviceInfo));
    }
}

void FindThread::onTimerTimeOut()
{
    steps++;
    emit(timerTimeOutSignal(steps));
}

void FindThread::onRcvThreadExitSig()
{
    this->exit();
    //this->quit();  //chat-gpt说两种写法都可以
}
