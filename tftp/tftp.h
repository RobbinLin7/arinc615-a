#ifndef TFTP_H
#define TFTP_H
#include<QObject>
#include<QFile>
#include<QUdpSocket>
#include "dir.h"
#define TFTP_NOT_LAST_DATA_LEN 512
#define MAX_RETRANS_TIMES 20
#define WAIT_TIME_MS 2000
class Tftp
{
public:
    Tftp();
    enum TftpMode{RRQ, WRQ};//区分读请求和写请求
    static bool sendFile(QUdpSocket *uSock, QString fileName, QString* errorMessage, enum TftpMode tftpMode = RRQ);
    static bool sendFile(QUdpSocket *uSock, QString fileName, QString* errorMessage, bool* mainThreadExitedOrNot, enum TftpMode tftpMode = RRQ);
    static bool sendFile_GBN(QUdpSocket *uSock, QString fileName, QString* errorMessage, bool* mainThreadExitedOrNot, enum TftpMode tftpMode = RRQ);
    //static bool sendFile(QUdpSocket *uSock, QString fileName, enum TftpMode tftpMode = RRQ);
    static bool receiveFile(QUdpSocket *uSock, QString fileName, QString* errorMessage, enum TftpMode tftpMode = RRQ);
    static bool receiveFile(QUdpSocket *uSock, QString fileName, QString* errorMessage, bool* mainThreadExitedOrNot, enum TftpMode tftpMode = RRQ);
    //static bool receiveFile(QUdpSocket *uSock, QString fileName, enum TftpMode tftpMode = RRQ);
    static QByteArray makeTftpData(char data[], int len, quint16 block);
    static QByteArray makeTftpReadRequest(QString fileName, QString mode = "octet");
    static QByteArray makeTftpWriteRequest(QString fileName, QString mode = "octet");
    static QByteArray makeTftpAck(quint16 block);
};


struct Packet{
    QByteArray data;
    unsigned int dataLen;
};

#endif // TFTP_H
