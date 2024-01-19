#ifndef TFTP_H
#define TFTP_H
#include<QObject>
#include<QFile>
#include<QUdpSocket>
#include "dir.h"
//#define TFTP_NOT_LAST_DATA_LEN 512
//#define MAX_RETRANS_TIMES 20
//#define WAIT_TIME_MS 2000

extern unsigned short blkSize;
extern unsigned short timeout;
extern unsigned short retrans_times;

class Tftp
{
public:
    Tftp();
    enum TftpPacketType{RRQ, WRQ, DATA, ACK, ERROR, OACK};
    //static bool sendFile(QUdpSocket *uSock, QString fileName, QString* errorMessage, bool* mainThreadExitedOrNot, enum TftpMode tftpMode = RRQ);
    //static bool sendFile_GBN(QUdpSocket *uSock, QString fileName, QString* errorMessage, bool* mainThreadExitedOrNot, enum TftpMode tftpMode = RRQ);
    //static bool receiveFile(QUdpSocket *uSock, QString fileName, QString* errorMessage, bool* mainThreadExitedOrNot, enum TftpMode tftpMode = RRQ);

    static void parseOACK(QByteArray &oackData);


    static bool put(QUdpSocket* uSock, QString path, QString fileName, QString* errorMessage, const QHostAddress& address, const quint16 port);
    static bool get(QUdpSocket* uSock, QString path, QString fileName, QString* errorMessage, const QHostAddress& address, const quint16 port);
    static bool handlePut(QUdpSocket* uSock, QString path, QString fileName, QString* errorMessage, const QHostAddress& address, const quint16 port);
    static bool handleGet(QUdpSocket* uSock, QString path, QString fileName, QString* errorMessage, const QHostAddress& address, const quint16 port);
    static bool downLoad(QUdpSocket* uSock, QString path, QString fileName, QString* errorMessage, const QHostAddress& address, const quint16 port, QByteArray lastPacketOfConsult);
    static bool upload(QUdpSocket* uSock, QString path, QString fileName, QString* errorMessage, const QHostAddress& address, const quint16 port, QByteArray lastPacketOfConsult);


    static QByteArray makeTftpData(char data[], int len, quint16 block);
    static QByteArray makeTftpReadRequest(QString fileName, QString mode = "octet", quint16 valueOfBlockSize = 512, quint16 valueOfTimeOut = 2);
    static QByteArray makeTftpWriteRequest(QString fileName, QString mode = "octet", quint16 valueOfBlockSize = 512, quint16 valueOfTimeOut = 2);
    static QByteArray makeTftpAck(quint16 block);
    static QByteArray makeTftpOAck(const QMap<QString,QString> &options);
    static QByteArray makeTftpError(quint16 errorCode, QString errorMessage);

    static TftpPacketType getTftpPacketType(const QByteArray& tftpPacket);
    static bool checkBlockNo(const QByteArray& tftpPacket, unsigned short expectedBlockNo);
};


struct Packet{
    QByteArray data;
    unsigned int dataLen;
};

#endif // TFTP_H
