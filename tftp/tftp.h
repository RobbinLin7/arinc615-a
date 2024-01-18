#ifndef TFTP_H
#define TFTP_H
#include<QObject>
#include<QFile>
#include<QUdpSocket>
#include "dir.h"
//#define TFTP_NOT_LAST_DATA_LEN 512
//#define MAX_RETRANS_TIMES 20
//#define WAIT_TIME_MS 2000
class Tftp
{
public:
    Tftp();
    enum TftpMode{RRQ, WRQ};//区分读请求和写请求
    //static bool sendFile(QUdpSocket *uSock, QString fileName, QString* errorMessage, bool* mainThreadExitedOrNot, enum TftpMode tftpMode = RRQ);
    //static bool sendFile_GBN(QUdpSocket *uSock, QString fileName, QString* errorMessage, bool* mainThreadExitedOrNot, enum TftpMode tftpMode = RRQ);
    //static bool receiveFile(QUdpSocket *uSock, QString fileName, QString* errorMessage, bool* mainThreadExitedOrNot, enum TftpMode tftpMode = RRQ);

    static bool put(QUdpSocket* uSock, QString fileName, QString* errorMessage);
    static bool get(QUdpSocket* uSock, QString fileName, QString* errorMessage);
    static bool handlePut(QUdpSocket* uSock, QString fileName, QString* errorMessage);
    static bool handleGet(QUdpSocket* uSock, QString fileName, QString* errorMessage);
    static QByteArray makeTftpData(char data[], int len, quint16 block);
    static QByteArray makeTftpReadRequest(QString fileName, QString mode = "octet", quint16 valueOfBlockSize = 512, quint8 valueOfTimeOut = 1);
    static QByteArray makeTftpWriteRequest(QString fileName, QString mode = "octet", quint16 valueOfBlockSize = 512, quint8 valueOfTimeOut = 1);
    static QByteArray makeTftpAck(quint16 block);
    static QByteArray makeTftpOAck(const QMap<QString,QString> &options);
    static QByteArray makeTftpError(quint16 errorCode, QString errorMessage);
};


struct Packet{
    QByteArray data;
    unsigned int dataLen;
};

#endif // TFTP_H
